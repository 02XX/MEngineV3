#include "RenderPassManager.hpp"
#include "Logger.hpp"
#include <vector>
#include <vulkan/vulkan.hpp>
namespace MEngine::Core::Manager
{

void RenderPassManager::CreateCompositionRenderPass()
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
    // SubPass 0: ForwardComposition
    std::vector<vk::AttachmentReference> colorRefs{
        vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal), // Render Target: Color
    };
    vk::AttachmentReference depthRef{
        vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal), // Render Target: Depth
    };
    vk::SubpassDescription forwardCompositionSubpass{};
    forwardCompositionSubpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachments(colorRefs)
        .setPDepthStencilAttachment(&depthRef);
    mSubPasses[RenderPassType::ForwardComposition] = 0; // SubPass 0

    // SubPass 1
    std::vector<vk::SubpassDescription> subpasses{
        forwardCompositionSubpass,
    };
    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.setAttachments(attachments).setSubpasses(subpasses);
    auto renderPass = mVulkanContext->GetDevice().createRenderPassUnique(renderPassCreateInfo);
    if (!renderPass)
    {
        LogError("Failed to create Forward render pass");
    }
    mCompositionRenderPass = std::move(renderPass);
    LogDebug("Forward render pass created successfully");
}
std::tuple<vk::RenderPass, uint32_t> RenderPassManager::GetRenderPass(RenderPassType type) const
{
    if (mSubPasses.contains(type))
    {
        return {mCompositionRenderPass.get(), mSubPasses.at(type)};
    }
    LogError("SubPass pass type {} not found", magic_enum::enum_name(type));
    throw std::runtime_error("SubPass pass type not found");
}

} // namespace MEngine::Core::Manager