#include "MRenderSystem.hpp"
#include "Logger.hpp"
#include "MMaterialComponent.hpp"
#include "MMeshComponent.hpp"
#include "MPipeline.hpp"
#include "MTexture.hpp"
#include "MTransformComponent.hpp"
#include "MTransformSystem.hpp"
#include "VulkanContext.hpp"
#include <cstdint>

namespace MEngine::Function::System
{
void MRenderSystem::Init()
{
    CreateRenderTarget();
    CreateFramebuffer();
    mImageAvailableSemaphores.resize(mFrameCount);
    mRenderFinishedSemaphores.resize(mFrameCount);
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        // 创建命令缓冲区
        vk::CommandBufferAllocateInfo graphicsCommandBufferAllocateInfo;
        graphicsCommandBufferAllocateInfo.setCommandPool(mContext->GetGraphicsCommandPool())
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        mGraphicsCommandBuffers.push_back(
            std::move(mContext->GetDevice().allocateCommandBuffersUnique(graphicsCommandBufferAllocateInfo)[0]));

        // 创建同步对象
        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        mInFlightFences.push_back(mContext->GetDevice().createFenceUnique(fenceCreateInfo));

        vk::SemaphoreCreateInfo semaphoreCreateInfo;
        mRenderFinishedSemaphores[i] = mContext->GetDevice().createSemaphoreUnique(semaphoreCreateInfo);
    }
}
void MRenderSystem::Update(float deltaTime)
{
    Batch();

    auto commandBuffer = mGraphicsCommandBuffers[mCurrentFrameIndex].get();
    auto inFlightFence = mInFlightFences[mCurrentFrameIndex].get();
    auto width = mRenderTargets[mCurrentFrameIndex].width;
    auto height = mRenderTargets[mCurrentFrameIndex].height;
    auto renderFinishedSemaphore = mRenderFinishedSemaphores[mCurrentFrameIndex].get();
    auto imageAvailableSemaphore = mImageAvailableSemaphores[mCurrentFrameIndex];
    Prepare(commandBuffer, inFlightFence);
    for (const auto &[pipelineID, entities] : mRenderQueue)
    {
        auto pipeline = mResourceManager->GetAsset<MPipeline>(pipelineID);
        auto renderPassType = pipeline->GetSetting().RenderPassType;
        switch (renderPassType)
        {
        case RenderPassType::ShadowDepth:
        case RenderPassType::DeferredComposition:
            break;
        case RenderPassType::ForwardComposition: {
            auto framebuffer = mFramebuffers[RenderPassType::ForwardComposition][mCurrentFrameIndex].get();
            RenderForwardCompositePass(commandBuffer, {width, height}, framebuffer, pipeline->GetPipeline(), entities);
            break;
        }
        case RenderPassType::Sky:
        case RenderPassType::Transparent:
        case RenderPassType::PostProcess:
        case RenderPassType::UI:
            break;
        }
    }
    End(commandBuffer, inFlightFence, renderFinishedSemaphore, nullptr);
    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mFrameCount;
}
void MRenderSystem::Shutdown()
{
}
void MRenderSystem::CreateRenderTarget()
{
    mRenderTargets.resize(mFrameCount);
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        auto colorTextureSetting = MTextureSetting{};
        colorTextureSetting.isRenderTarget = true;
        colorTextureSetting.format = vk::Format::eR32G32B32A32Sfloat;
        colorTextureSetting.width = mRenderTargets[i].width;
        colorTextureSetting.height = mRenderTargets[i].height;
        mRenderTargets[i].colorTexture = mResourceManager->CreateAsset<MTexture, MTextureSetting>(colorTextureSetting);
        auto depthStencilTextureSetting = MTextureSetting{};
        depthStencilTextureSetting.isRenderTarget = true;
        depthStencilTextureSetting.format = vk::Format::eD32SfloatS8Uint;
        depthStencilTextureSetting.width = mRenderTargets[i].width;
        depthStencilTextureSetting.height = mRenderTargets[i].height;
        mRenderTargets[i].depthStencilTexture =
            mResourceManager->CreateAsset<MTexture, MTextureSetting>(depthStencilTextureSetting);
    }
}
void MRenderSystem::CreateFramebuffer()
{
    mFramebuffers[RenderPassType::ForwardComposition].resize(mFrameCount);
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        std::vector<vk::ImageView> attachments;
        attachments.push_back(mRenderTargets[i].colorTexture->GetImageView());
        attachments.push_back(mRenderTargets[i].depthStencilTexture->GetImageView());
        // 创建Framebuffer
        vk::FramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.setRenderPass(mRenderPassManager->GetRenderPass(RenderPassType::ForwardComposition))
            .setAttachments(attachments)
            .setWidth(mRenderTargets[i].width)
            .setHeight(mRenderTargets[i].height)
            .setLayers(1);
        mFramebuffers[RenderPassType::ForwardComposition][i] =
            mContext->GetDevice().createFramebufferUnique(framebufferCreateInfo);
    }
}
void MRenderSystem::Batch()
{
    mRenderQueue.clear();

    auto view = mRegistry->view<MTransformComponent, MMeshComponent, MMaterialComponent>();
    for (auto &entity : view)
    {
        auto &transformComponent = view.get<MTransformComponent>(entity);
        auto &meshComponent = view.get<MMeshComponent>(entity);
        auto &materialComponent = view.get<MMaterialComponent>(entity);
        mRenderQueue[materialComponent.material->GetPipelineID()].push_back(entity);
    }
}
void MRenderSystem::Prepare(vk::CommandBuffer commandBuffer, vk::Fence fence)
{
    auto result = mContext->GetDevice().waitForFences({fence}, vk::True,
                                                      1000000000); // 1s
    if (result != vk::Result::eSuccess)
    {
        LogError("MRenderSystem::Prepare", "Failed to wait for in-flight fence: {}", vk::to_string(result));
        throw std::runtime_error("Failed to wait fence");
    }
    mContext->GetDevice().resetFences({fence});
    commandBuffer.reset();
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);
}
void MRenderSystem::RenderForwardCompositePass(vk::CommandBuffer commandBuffer, vk::Extent2D extent,
                                               vk::Framebuffer framebuffer, vk::Pipeline pipeline,
                                               const std::vector<entt::entity> &entities)
{
    vk::RenderPassBeginInfo renderPassBeginInfo;
    auto clearValues = RenderTarget::GetClearValues();
    auto width = extent.width;
    auto height = extent.height;
    renderPassBeginInfo.setRenderPass(mRenderPassManager->GetRenderPass(RenderPassType::ForwardComposition))
        .setFramebuffer(framebuffer)
        .setRenderArea({{0, 0}, {width, height}})
        .setClearValues(clearValues);
    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(0.0f)
        .setWidth(static_cast<float>(width))
        .setHeight(static_cast<float>(height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    commandBuffer.setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({width, height});
    commandBuffer.setScissor(0, {scissor});
    commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    for (const entt::entity &entity : entities)
    {
        auto &materialComponent = mRegistry->get<MMaterialComponent>(entity);
        auto &meshComponent = mRegistry->get<MMeshComponent>(entity);
        auto &transformComponent = mRegistry->get<MTransformComponent>(entity);
        // 1. 绑定 push_constants
        // 2. 绑定Global描述符集
        // 3. 绑定材质描述符集
        // 4. 绑定顶点缓冲区
        auto vertexBuffer = meshComponent.mesh->GetVertexBuffer();
        commandBuffer.bindVertexBuffers(0, vertexBuffer, {0});
        // 5. 绑定索引缓冲区
        auto indexBuffer = meshComponent.mesh->GetIndexBuffer();
        commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
        // 6. 绘制Draw Call
        commandBuffer.drawIndexed(meshComponent.mesh->GetIndexCount(), 1, 0, 0, 0);
    }
    // commandBuffer.nextSubpass(vk::SubpassContents::eInline);
    commandBuffer.endRenderPass();
}
void MRenderSystem::End(vk::CommandBuffer commandBuffer, vk::Fence fence, vk::Semaphore signalSemaphore,
                        vk::Semaphore waitSemaphore)
{
    commandBuffer.end();
    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setCommandBuffers(commandBuffer).setSignalSemaphores(signalSemaphore);
    mContext->GetGraphicsQueue().submit(submitInfo, fence);
}
} // namespace MEngine::Function::System