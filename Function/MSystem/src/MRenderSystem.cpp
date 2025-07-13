#include "MRenderSystem.hpp"
#include "IMTextureManager.hpp"
#include "Logger.hpp"
#include "MCameraComponent.hpp"
#include "MMaterialComponent.hpp"
#include "MMeshComponent.hpp"
#include "MPipeline.hpp"
#include "MPipelineManager.hpp"
#include "MTexture.hpp"
#include "MTransformComponent.hpp"
#include "MTransformSystem.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <cstring>
#include <glm/fwd.hpp>
#include <vector>
namespace MEngine::Function::System
{
void MRenderSystem::Init()
{
    CreateRenderTarget();
    CreateFramebuffer();
    CreateEnvironmentMap();
    mImageAvailableSemaphores.resize(mFrameCount);
    mRenderFinishedSemaphores.resize(mFrameCount);
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        // 创建命令缓冲区
        vk::CommandBufferAllocateInfo graphicsCommandBufferAllocateInfo;
        graphicsCommandBufferAllocateInfo.setCommandPool(mVulkanContext->GetGraphicsCommandPool())
            .setLevel(vk::CommandBufferLevel::ePrimary)
            .setCommandBufferCount(1);
        mGraphicsCommandBuffers.push_back(
            std::move(mVulkanContext->GetDevice().allocateCommandBuffersUnique(graphicsCommandBufferAllocateInfo)[0]));

        // 创建同步对象
        vk::FenceCreateInfo fenceCreateInfo;
        fenceCreateInfo.setFlags(vk::FenceCreateFlagBits::eSignaled);
        mInFlightFences.push_back(mVulkanContext->GetDevice().createFenceUnique(fenceCreateInfo));

        vk::SemaphoreCreateInfo semaphoreCreateInfo;
        mRenderFinishedSemaphores[i] = mVulkanContext->GetDevice().createSemaphoreUnique(semaphoreCreateInfo);

        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo;
        auto globalDescriptorSetLayouts =
            std::vector<vk::DescriptorSetLayout>{mPipelineManager->GetGlobalDescriptorSetLayout()};
        descriptorSetAllocateInfo.setDescriptorPool(mVulkanContext->GetDescriptorPool())
            .setDescriptorSetCount(1)
            .setSetLayouts(globalDescriptorSetLayouts);
        auto descriptorSets = mVulkanContext->GetDevice().allocateDescriptorSetsUnique(descriptorSetAllocateInfo);
        if (!descriptorSets.empty())
        {
            mGlobalDescriptorSets.push_back(std::move(descriptorSets[0]));
        }
    }
    vk::BufferCreateInfo cameraUBOCreateInfo{};
    cameraUBOCreateInfo.setSize(sizeof(CameraParameters))
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo cameraUBOAllocationCreateInfo{};
    cameraUBOAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    cameraUBOAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(cameraUBOCreateInfo),
                        &cameraUBOAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&mCameraUBO),
                        &mCameraUBOAllocation, &mCameraUBOAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create camera UBO");
        throw std::runtime_error("Failed to create camera UBO");
    }
    vk::BufferCreateInfo lightUBOCreateInfo{};
    lightUBOCreateInfo.setSize(sizeof(LightParameters) * MAX_LIGHT_COUNT)
        .setUsage(vk::BufferUsageFlagBits::eUniformBuffer | vk::BufferUsageFlagBits::eTransferDst)
        .setSharingMode(vk::SharingMode::eExclusive);
    VmaAllocationCreateInfo lightUBOAllocationCreateInfo{};
    lightUBOAllocationCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    lightUBOAllocationCreateInfo.flags =
        VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
    if (vmaCreateBuffer(mVulkanContext->GetVmaAllocator(), &static_cast<VkBufferCreateInfo &>(lightUBOCreateInfo),
                        &lightUBOAllocationCreateInfo, reinterpret_cast<VkBuffer *>(&mLightUBO), &mLightUBOAllocation,
                        &mLightUBOAllocationInfo) != VK_SUCCESS)
    {
        LogError("Failed to create light UBO");
        throw std::runtime_error("Failed to create light UBO");
    }
}
void MRenderSystem::Update(float deltaTime)
{
    Batch();
    Prepare();
    RenderForwardCompositePass();
    End();
    mCurrentFrameIndex = (mCurrentFrameIndex + 1) % mFrameCount;
}
void MRenderSystem::Shutdown()
{
    if (mCameraUBO)
    {
        vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mCameraUBO, mCameraUBOAllocation);
    }
    if (mLightUBO)
    {
        vmaDestroyBuffer(mVulkanContext->GetVmaAllocator(), mLightUBO, mLightUBOAllocation);
    }
}
void MRenderSystem::CreateRenderTarget()
{
    mRenderTargets.resize(mFrameCount);
    auto textureManager = mResourceManager->GetManager<MTexture, IMTextureManager>();
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        mRenderTargets[i].colorTexture =
            textureManager->CreateColorAttachment(mRenderTargets[i].width, mRenderTargets[i].height);
        mRenderTargets[i].depthStencilTexture =
            textureManager->CreateDepthStencilAttachment(mRenderTargets[i].width, mRenderTargets[i].height);
    }
}
void MRenderSystem::CreateFramebuffer()
{
    mFramebuffers.resize(mFrameCount);
    for (uint32_t i = 0; i < mFrameCount; ++i)
    {
        std::vector<vk::ImageView> attachments;
        attachments.push_back(mRenderTargets[i].colorTexture->GetImageView());
        attachments.push_back(mRenderTargets[i].depthStencilTexture->GetImageView());
        // 创建Framebuffer
        vk::FramebufferCreateInfo framebufferCreateInfo;
        framebufferCreateInfo.setRenderPass(mRenderPassManager->GetCompositionRenderPass())
            .setAttachments(attachments)
            .setWidth(mRenderTargets[i].width)
            .setHeight(mRenderTargets[i].height)
            .setLayers(1);
        mFramebuffers[i] = mVulkanContext->GetDevice().createFramebufferUnique(framebufferCreateInfo);
    }
}
void MRenderSystem::CreateEnvironmentMap()
{
    auto textureManager = mResourceManager->GetManager<MTexture, IMTextureManager>();
    mEnvironmentMap = textureManager->GetDefaultTexture(DefaultTextureType::EnvironmentMap);
    mIrradianceMap = textureManager->GetDefaultTexture(DefaultTextureType::IrradianceMap);
    mBRDFLUT = textureManager->GetDefaultTexture(DefaultTextureType::BRDFLUT);
}
void MRenderSystem::ReSizeFrameBuffer(uint32_t width, uint32_t height)
{
    mVulkanContext->GetDevice().waitIdle();
    for (auto &renderTarget : mRenderTargets)
    {
        renderTarget.height = height;
        renderTarget.width = width;
    }
    CreateRenderTarget();
    CreateFramebuffer();
    LogInfo("Frame buffer resized to {}x{}", width, height);
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
        auto renderPassType = materialComponent.material->GetPipeline()->GetSetting().RenderPassType;
        auto pipeline = materialComponent.material->GetPipeline();
        mRenderQueue[renderPassType][pipeline].push_back(entity);
    }
}
void MRenderSystem::Prepare()
{
    auto commandBuffer = mGraphicsCommandBuffers[mCurrentFrameIndex].get();
    auto fence = mInFlightFences[mCurrentFrameIndex].get();
    // 相机
    auto cameraView = mRegistry->view<MTransformComponent, MCameraComponent>();
    for (auto camera : cameraView)
    {
        auto &transformComponent = cameraView.get<MTransformComponent>(camera);
        auto &cameraComponent = cameraView.get<MCameraComponent>(camera);
        if (cameraComponent.isMainCamera)
        {
            mCameraParameters.Position = transformComponent.worldPosition;
            mCameraParameters.Direction = transformComponent.worldRotation * glm::vec3(0.0f, 0.0f, -1.0f);
            mCameraParameters.ViewMatrix = cameraComponent.viewMatrix;
            mCameraParameters.ProjectionMatrix = cameraComponent.projectionMatrix;
        }
    }
    // 光照
    auto lightView = mRegistry->view<MTransformComponent, MLightComponent>();
    uint32_t lightCount = 0;
    for (auto light : lightView)
    {
        auto &transformComponent = lightView.get<MTransformComponent>(light);
        auto &lightComponent = lightView.get<MLightComponent>(light);
        LightParameters lightParams;
        lightParams.Position = transformComponent.worldPosition;
        lightParams.Direction = transformComponent.worldRotation * glm::vec3(0.0f, 0.0f, 1.0f);
        lightParams.Color = lightComponent.Color;
        lightParams.Intensity = lightComponent.Intensity;
        lightParams.Radius = lightComponent.Radius;
        lightParams.InnerConeAngle = lightComponent.InnerConeAngle;
        lightParams.OuterConeAngle = lightComponent.OuterConeAngle;
        lightParams.LightType = lightComponent.LightType;
        lightParams.enable = 1; // 启用光源
        mLightParameters[lightCount] = lightParams;
        lightCount++;
    }
    auto result = mVulkanContext->GetDevice().waitForFences({fence}, vk::True,
                                                            1000000000); // 1s
    if (result != vk::Result::eSuccess)
    {
        LogError("MRenderSystem::Prepare", "Failed to wait for in-flight fence: {}", vk::to_string(result));
        throw std::runtime_error("Failed to wait fence");
    }
    mVulkanContext->GetDevice().resetFences({fence});
    commandBuffer.reset();
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    commandBuffer.begin(beginInfo);
    WriteGlobalDescriptorSet(mCurrentFrameIndex);
}
void MRenderSystem::RenderForwardCompositePass()
{
    auto commandBuffer = mGraphicsCommandBuffers[mCurrentFrameIndex].get();
    auto extent = mRenderTargets[mCurrentFrameIndex].GetExtent();
    auto framebuffer = mFramebuffers[mCurrentFrameIndex].get();
    auto renderPassType = RenderPassType::ForwardComposition;
    auto &&[renderPass, subpass] = mRenderPassManager->GetRenderPass(renderPassType);
    auto globalDescriptorSet = mGlobalDescriptorSets[mCurrentFrameIndex].get();

    vk::RenderPassBeginInfo renderPassBeginInfo;
    auto clearValues = RenderTarget::GetClearValues();
    auto width = extent.width;
    auto height = extent.height;
    renderPassBeginInfo.setRenderPass(renderPass)
        .setFramebuffer(framebuffer)
        .setRenderArea({{0, 0}, {width, height}})
        .setClearValues(clearValues);

    commandBuffer.beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);
    vk::Viewport viewport;
    viewport.setX(0.0f)
        .setY(height)
        .setWidth(static_cast<float>(width))
        .setHeight(-static_cast<float>(height))
        .setMinDepth(0.0f)
        .setMaxDepth(1.0f);
    commandBuffer.setViewport(0, {viewport});
    vk::Rect2D scissor;
    scissor.setOffset({0, 0}).setExtent({width, height});
    commandBuffer.setScissor(0, {scissor});
    for (const auto &[pipeline, entities] : mRenderQueue[renderPassType])
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline->GetPipeline());
        for (auto entity : entities)
        {
            auto &materialComponent = mRegistry->get<MMaterialComponent>(entity);
            auto &meshComponent = mRegistry->get<MMeshComponent>(entity);
            auto &transformComponent = mRegistry->get<MTransformComponent>(entity);
            // 1. 绑定 push_constants
            commandBuffer.pushConstants(materialComponent.material->GetPipeline()->GetPipelineLayout(),
                                        vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment, 0,
                                        sizeof(glm::mat4), &transformComponent.modelMatrix);
            // 2. 绑定Global描述符集
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             materialComponent.material->GetPipeline()->GetPipelineLayout(), 0,
                                             globalDescriptorSet, {});
            // 3. 绑定材质描述符集
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                             materialComponent.material->GetPipeline()->GetPipelineLayout(), 1,
                                             materialComponent.material->GetMaterialDescriptorSet(), {});
            // 4. 绑定顶点缓冲区
            auto vertexBuffer = meshComponent.mesh->GetVertexBuffer();
            commandBuffer.bindVertexBuffers(0, vertexBuffer, {0});
            // 5. 绑定索引缓冲区
            auto indexBuffer = meshComponent.mesh->GetIndexBuffer();
            commandBuffer.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
            // 6. 绘制Draw Call
            commandBuffer.drawIndexed(meshComponent.mesh->GetIndexCount(), 1, 0, 0, 0);
        }
    }
    // commandBuffer.nextSubpass(vk::SubpassContents::eInline);
    commandBuffer.endRenderPass();
}
void MRenderSystem::End()
{
    auto commandBuffer = mGraphicsCommandBuffers[mCurrentFrameIndex].get();
    auto fence = mInFlightFences[mCurrentFrameIndex].get();
    auto signalSemaphore = mRenderFinishedSemaphores[mCurrentFrameIndex].get();
    auto waitSemaphore = mImageAvailableSemaphores[mCurrentFrameIndex];
    commandBuffer.end();
    vk::SubmitInfo submitInfo;
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    submitInfo.setCommandBuffers(commandBuffer).setSignalSemaphores(signalSemaphore);
    mVulkanContext->GetGraphicsQueue().submit(submitInfo, fence);
}
void MRenderSystem::WriteGlobalDescriptorSet(uint32_t globalDescriptorSetIndex)
{
    memcpy(mCameraUBOAllocationInfo.pMappedData, &mCameraParameters, sizeof(CameraParameters));
    memcpy(mLightUBOAllocationInfo.pMappedData, mLightParameters.data(), sizeof(LightParameters) * MAX_LIGHT_COUNT);

    // 更新全局描述集
    std::vector<vk::WriteDescriptorSet> writeDescriptorSets;
    writeDescriptorSets.resize(mPipelineManager->GetGlobalDescriptorSetLayoutBindings().size());
    vk::DescriptorBufferInfo cameraParamsBufferInfo;
    cameraParamsBufferInfo.setBuffer(mCameraUBO).setOffset(0).setRange(sizeof(CameraParameters));
    writeDescriptorSets[0]
        .setBufferInfo(cameraParamsBufferInfo)
        .setDstSet(mGlobalDescriptorSets[globalDescriptorSetIndex].get())
        .setDstBinding(0)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1);
    vk::DescriptorBufferInfo lightParamsBufferInfo;

    lightParamsBufferInfo.setBuffer(mLightUBO).setOffset(0).setRange(sizeof(LightParameters) * MAX_LIGHT_COUNT);

    writeDescriptorSets[1]
        .setBufferInfo(lightParamsBufferInfo)
        .setDstSet(mGlobalDescriptorSets[globalDescriptorSetIndex].get())
        .setDstBinding(1)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(1);

    vk::DescriptorImageInfo environmentMapImageInfo;
    environmentMapImageInfo.setImageView(mEnvironmentMap->GetImageView())
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSampler(mEnvironmentMap->GetSampler())
        .setImageView(mEnvironmentMap->GetImageView());
    writeDescriptorSets[2]
        .setImageInfo(environmentMapImageInfo)
        .setDstSet(mGlobalDescriptorSets[globalDescriptorSetIndex].get())
        .setDstBinding(2)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1);
    vk::DescriptorImageInfo irradianceMapImageInfo;
    irradianceMapImageInfo.setImageView(mIrradianceMap->GetImageView())
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSampler(mIrradianceMap->GetSampler())
        .setImageView(mIrradianceMap->GetImageView());
    writeDescriptorSets[3]
        .setImageInfo(irradianceMapImageInfo)
        .setDstSet(mGlobalDescriptorSets[globalDescriptorSetIndex].get())
        .setDstBinding(3)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1);
    vk::DescriptorImageInfo brdfLUTImageInfo;
    brdfLUTImageInfo.setImageView(mBRDFLUT->GetImageView())
        .setImageLayout(vk::ImageLayout::eShaderReadOnlyOptimal)
        .setSampler(mBRDFLUT->GetSampler())
        .setImageView(mBRDFLUT->GetImageView());
    writeDescriptorSets[4]
        .setImageInfo(brdfLUTImageInfo)
        .setDstSet(mGlobalDescriptorSets[globalDescriptorSetIndex].get())
        .setDstBinding(4)
        .setDstArrayElement(0)
        .setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
        .setDescriptorCount(1);

    mVulkanContext->GetDevice().updateDescriptorSets(writeDescriptorSets, {});
}
} // namespace MEngine::Function::System