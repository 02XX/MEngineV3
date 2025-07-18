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
        // 2: Normal Map
        vk::AttachmentDescription()
            .setFormat(vk::Format::eR32G32B32A32Sfloat)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal),
        // 3: ARM Map
        vk::AttachmentDescription()
            .setFormat(vk::Format::eR32G32B32A32Sfloat)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal),
        // 4. Position Map
        vk::AttachmentDescription()
            .setFormat(vk::Format::eR32G32B32A32Sfloat)
            .setSamples(vk::SampleCountFlagBits::e1)
            .setLoadOp(vk::AttachmentLoadOp::eClear)
            .setStoreOp(vk::AttachmentStoreOp::eStore)
            .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
            .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
            .setInitialLayout(vk::ImageLayout::eUndefined)
            .setFinalLayout(vk::ImageLayout::eShaderReadOnlyOptimal),
    };
    // SubPass 0: GBuffer
    std::vector<vk::AttachmentReference> gBufferColorRefs{
        vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal), // Render Target: Color
        vk::AttachmentReference(2, vk::ImageLayout::eColorAttachmentOptimal), // Normal Map
        vk::AttachmentReference(3, vk::ImageLayout::eColorAttachmentOptimal), // ARM Map
        vk::AttachmentReference(4, vk::ImageLayout::eColorAttachmentOptimal), // Position Map
    };
    vk::AttachmentReference gBufferDepthRef{
        vk::AttachmentReference(1, vk::ImageLayout::eDepthStencilAttachmentOptimal), // Render Target: Depth
    };
    vk::SubpassDescription gBufferSubpass{};
    gBufferSubpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachments(gBufferColorRefs)
        .setPDepthStencilAttachment(&gBufferDepthRef);
    mSubPasses[RenderPassType::GBuffer] = 0; // SubPass 0
    // SubPass 1: Lighting
    std::vector<vk::AttachmentReference> lightingColorRefs{
        vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal), // Render Target: Color
    };
    std::vector<vk::AttachmentReference> lightingInputRefs{
        vk::AttachmentReference(2, vk::ImageLayout::eShaderReadOnlyOptimal), // Normal Map
        vk::AttachmentReference(3, vk::ImageLayout::eShaderReadOnlyOptimal), // ARM Map
        vk::AttachmentReference(4, vk::ImageLayout::eShaderReadOnlyOptimal), // Position Map
    };
    vk::SubpassDescription lightingSubpass{};
    lightingSubpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachments(lightingColorRefs)
        .setInputAttachments(lightingInputRefs)
        .setPDepthStencilAttachment(&gBufferDepthRef);
    mSubPasses[RenderPassType::Lighting] = 1; // SubPass 1
    // SubPass 2: Forward
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
    mSubPasses[RenderPassType::ForwardComposition] = 2; // SubPass 2
    // SubPass 3: Sky RenderPass
    std::vector<vk::AttachmentReference> environmentColorRefs{
        vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal), // Render Target: Color
    };
    vk::SubpassDescription environmentSubpass{};
    environmentSubpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
        .setColorAttachments(environmentColorRefs)
        .setPDepthStencilAttachment(&depthRef);
    mSubPasses[RenderPassType::Sky] = 3; // SubPass 3
    // Create Render Pass
    std::vector<vk::SubpassDescription> subpasses{
        gBufferSubpass,
        lightingSubpass,
        forwardCompositionSubpass,
        environmentSubpass,
    };
    // subpass dependencies
    std::vector<vk::SubpassDependency> dependencies{
        // Subpass 0 -> Subpass 1
        vk::SubpassDependency()
            .setSrcSubpass(0)
            .setDstSubpass(1)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead),
        // Subpass 1 -> Subpass 2
        vk::SubpassDependency()
            .setSrcSubpass(1)
            .setDstSubpass(2)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead),
        // Subpass 2 -> Subpass 3
        vk::SubpassDependency()
            .setSrcSubpass(2)
            .setDstSubpass(3)
            .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
            .setDstStageMask(vk::PipelineStageFlagBits::eFragmentShader)
            .setSrcAccessMask(vk::AccessFlagBits::eColorAttachmentWrite)
            .setDstAccessMask(vk::AccessFlagBits::eShaderRead),
    };
    vk::RenderPassCreateInfo renderPassCreateInfo{};
    renderPassCreateInfo.setAttachments(attachments).setSubpasses(subpasses).setDependencies(dependencies);
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