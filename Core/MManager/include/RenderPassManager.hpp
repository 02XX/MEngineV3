#pragma once
#include "IMTextureManager.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <memory>
#include <unordered_map>
namespace MEngine::Core::Manager
{
enum class RenderPassType
{
    ShadowDepth,         // 生成所有光源的阴影贴图subpass0: 生成阴影贴图
    DeferredComposition, // Deferred, 延迟渲染Subpass0: GBuffer, Subpass1: Lighting
    ForwardComposition,  // Forward, 前向渲染subpass0: 不透明物体渲染 subpass1: 透明物体渲染，
                         // 创建多个MRT，Phong只渲染第一个MRT，PBR渲染所有MRT
    Sky,                 // 天空盒渲染subpass0: 天空盒渲染
    Transparent,         // Forward 透明物体渲染subpass0: 透明物体渲染
    PostProcess,         // 后处理渲染subpass0: 后处理渲染
    UI,                  // UI渲染subpass0: UI渲染
};
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
    RenderPassManager(std::shared_ptr<VulkanContext> vulkanContext)
        : mVulkanContext(vulkanContext)
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