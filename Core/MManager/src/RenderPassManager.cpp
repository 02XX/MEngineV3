#include "RenderPassManager.hpp"
#include "Logger.hpp"
#include <vulkan/vulkan.hpp>
namespace MEngine::Core::Manager
{

void RenderPassManager::CreateShadowDepthRenderPass()
{
}
void RenderPassManager::CreateDeferredCompositionRenderPass()
{
}
void RenderPassManager::CreateForwardCompositionRenderPass()
{
    std::vector<vk::AttachmentDescription> attachments{
        // 0：Render Target: Color
        vk::AttachmentDescription()
            .setFormat(GetRenderTargetFormat())
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eColorAttachmentOptimal),
        // 1: Render Target: Depth
        vk::AttachmentDescription()
            .setFormat(GetDepthStencilFormat()) // 32位深度+8位模板存储
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal),
    };
    // SubPass: 0 不透明物体
    std::vector<vk::AttachmentReference> colorRefs{
        vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal), // Render Target: Color
    };
    vk::AttachmentReference depthRef{
        vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal), // Render Target: Depth
    };
    // SubPass: 1 透明物体
    std::vector<vk::AttachmentReference> colorRefs1{
        vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal), // Render Target: Color
    };
    vk::AttachmentReference depthRef1{
        vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal), // Render Target: Depth
    };
    std::vector<vk::SubpassDescription> subpasses{
        vk::SubpassDescription()
            .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
            .setColorAttachments(colorRefs)
            .setPDepthStencilAttachment(&depthRef),
        //   vk::SubpassDescription()
        //       .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        //       .setColorAttachments(colorRefs1)
        //       .setPDepthStencilAttachment(&depthRef1)

    };
    // // dependency
    // std::vector<vk::SubpassDependency> dependencies{
    //     // subpass 0 -> subpass 0 (self-dependency)
    //     vk::SubpassDependency()
    //         .setSrcSubpass(vk::SubpassExternal)
    //         .setDstSubpass(0)
    //         .setSrcStageMask(vk::PipelineStageFlagBits::eNone)
    //         .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    //         .setSrcAccessMask(vk::AccessFlagBits::eNone)
    //         .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite),

    //     // subpass 0 -> subpass 1
    //     vk::SubpassDependency()
    //         .setSrcSubpass(0)
    //         .setDstSubpass(1)
    //         .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    //         .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    //         .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
    //         .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite),

    //     // subpass 1 -> subpass 1 (self-dependency)
    //     vk::SubpassDependency()
    //         .setSrcSubpass(1)
    //         .setDstSubpass(vk::SubpassExternal)
    //         .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
    //         .setDstStageMask(vk::PipelineStageFlagBits::eNone)
    //         .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eColorAttachmentRead)
    //         .setDstAccessMask(vk::AccessFlagBits::eNone),
    // };
    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.setAttachments(attachments).setSubpasses(subpasses);
    auto renderPass = mVulkanContext->GetDevice().createRenderPassUnique(renderPassCreateInfo);
    if (!renderPass)
    {
        LogError("Failed to create Forward render pass");
    }
    mRenderPasses[RenderPassType::ForwardComposition] = std::move(renderPass);
    LogDebug("Forward render pass created successfully");
}
void RenderPassManager::CreateSkyRenderPass()
{
}
void RenderPassManager::CreateTransparentRenderPass()
{
}
void RenderPassManager::CreatePostProcessRenderPass()
{
}
void RenderPassManager::CreateUIRenderPass()
{
}
vk::RenderPass RenderPassManager::GetRenderPass(RenderPassType type) const
{
    auto it = mRenderPasses.find(type);
    if (it != mRenderPasses.end())
    {
        return it->second.get();
    }
    else
    {
        LogError("Render pass not found for type {}", magic_enum::enum_name(type));
        return nullptr;
    }
}

} // namespace MEngine::Core::Manager