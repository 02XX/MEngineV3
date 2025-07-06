#pragma once
#include "MPipeline.hpp"
#include "VulkanContext.hpp"
#include <memory>
#include <unordered_map>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
// struct RenderTarget
// {
//     // Render target 0: Color
//     Asset::MTexture colorTexture;
//     // Render target 1: Depth/Stencil
//     Asset::MTexture depthStencilTexture;
//     // Render target 2: Albedo
//     Asset::MTexture albedoTexture;
//     // Render target 3: Normal
//     Asset::MTexture normalTexture;
//     // Render target 4: WorldPos
//     Asset::MTexture worldPosTexture;
//     // Render target 5: ARM (Ambient Occlusion, Roughness, Metallic)
//     Asset::MTexture armTexture;
//     // Render target 6: Emissive
//     Asset::MTexture emissiveTexture;
// };
class RenderPassManager final
{
  private:
    // DI
    std::shared_ptr<VulkanContext> mVulkanContext;

  private:
    std::unordered_map<RenderPassType, vk::UniqueRenderPass> mRenderPasses;

  private:
    void CreateShadowDepthRenderPass();
    void CreateDeferredCompositionRenderPass();
    void CreateForwardCompositionRenderPass();
    void CreateSkyRenderPass();
    void CreateTransparentRenderPass();
    void CreatePostProcessRenderPass();
    void CreateUIRenderPass();
    inline vk::Format GetRenderTargetFormat()
    {
        return vk::Format::eR32G32B32A32Sfloat; // 32位浮点数RGBA
    }
    inline vk::Format GetDepthStencilFormat()
    {
        return vk::Format::eD32SfloatS8Uint; // 32位深度+8位模板存储
    }

  public:
    RenderPassManager(std::shared_ptr<VulkanContext> vulkanContext) : mVulkanContext(vulkanContext)
    {
        CreateShadowDepthRenderPass();
        CreateDeferredCompositionRenderPass();
        CreateForwardCompositionRenderPass();
        CreateSkyRenderPass();
        CreateTransparentRenderPass();
        CreatePostProcessRenderPass();
        CreateUIRenderPass();
    }
    vk::RenderPass GetRenderPass(RenderPassType type) const;
};

} // namespace MEngine::Core::Manager