#pragma once
#include "MPipeline.hpp"
#include "MSystem.hpp"
#include "MTexture.hpp"
#include "RenderPassManager.hpp"
#include "ResourceManager.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace MEngine::Function::System
{
class MRenderSystem final : public MSystem
{
  private:
    std::shared_ptr<VulkanContext> mContext;
    std::shared_ptr<RenderPassManager> mRenderPassManager;

  private:
    uint32_t mFrameCount{1};
    uint32_t mCurrentFrameIndex{0};
    std::vector<vk::UniqueCommandBuffer> mGraphicsCommandBuffers;
    std::vector<vk::UniqueFence> mInFlightFences;
    std::vector<vk::Semaphore> mImageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores;
    std::unordered_map<UUID, std::vector<entt::entity>> mRenderQueue;

    struct RenderTarget
    {
        uint32_t width{1280};
        uint32_t height{720};
        // Render target 0: Color
        std::shared_ptr<MTexture> colorTexture;

        // Render target 1: Depth/Stencil
        std::shared_ptr<MTexture> depthStencilTexture;

        // // Render target 2: Albedo
        // std::shared_ptr<MTexture> albedoTexture;
        // vk::ClearValue albedoClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})};
        // // Render target 3: Normal
        // std::shared_ptr<MTexture> normalTexture;
        // vk::ClearValue normalClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 1.0f, 1.0f})};
        // // Render target 4: WorldPos
        // std::shared_ptr<MTexture> worldPosTexture;
        // vk::ClearValue worldPosClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})};
        // // Render target 5: ARM (Ambient Occlusion, Roughness, Metallic)
        // std::shared_ptr<MTexture> armTexture;
        // vk::ClearValue armClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})};
        // // Render target 6: Emissive
        // Asset::MTexture emissiveTexture;
        // vk::ClearValue emissiveClearValue{vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f})};
        static std::vector<vk::ClearValue> GetClearValues()
        {
            vk::ClearValue colorClearValue{vk::ClearColorValue(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f})};
            vk::ClearValue depthClearValue{vk::ClearDepthStencilValue(1.0f, 0)};
            return {
                colorClearValue, depthClearValue
                // , albedoClearValue, normalClearValue, worldPosClearValue, armClearValue, emissiveClearValue
            };
        }
    };
    std::vector<RenderTarget> mRenderTargets;
    std::unordered_map<RenderPassType, std::vector<vk::UniqueFramebuffer>> mFramebuffers;

  public:
    MRenderSystem(std::shared_ptr<VulkanContext> context, std::shared_ptr<entt::registry> registry,
                  std::shared_ptr<ResourceManager> resourceManager,
                  std::shared_ptr<RenderPassManager> renderPassManager)
        : MSystem(registry, resourceManager), mContext(context), mRenderPassManager(renderPassManager)
    {
    }
    ~MRenderSystem() override = default;
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    inline void SetFrameCount(uint32_t count)
    {
        mFrameCount = count;
    }
    void SetExtent(uint32_t width, uint32_t height)
    {
        for (auto &renderTarget : mRenderTargets)
        {
            renderTarget.width = width;
            renderTarget.height = height;
        }
    }
    inline RenderTarget &GetRenderTarget(uint32_t index)
    {
        return mRenderTargets[index];
    }
    inline vk::Semaphore GetRenderFinishedSemaphore(uint32_t index) const
    {
        return mRenderFinishedSemaphores[index].get();
    }
    inline uint32_t GetCurrentFrameIndex() const
    {
        return mCurrentFrameIndex;
    }

  private:
    // void RenderShadowPass();
    void CreateRenderTarget();
    void CreateFramebuffer();
    void Batch();
    void Prepare(vk::CommandBuffer commandBuffer, vk::Fence fence);
    void RenderForwardCompositePass(vk::CommandBuffer commandBuffer, vk::Extent2D extent, vk::Framebuffer framebuffer,
                                    vk::Pipeline pipeline, const std::vector<entt::entity> &entities);
    void End(vk::CommandBuffer commandBuffer, vk::Fence fence, vk::Semaphore signalSemaphore,
             vk::Semaphore waitSemaphore);
    // void RenderPostProcessPass();
};
} // namespace MEngine::Function::System