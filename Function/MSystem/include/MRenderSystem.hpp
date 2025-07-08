#pragma once
#include "IMPipelineManager.hpp"
#include "MPipeline.hpp"
#include "MSystem.hpp"
#include "MTexture.hpp"
#include "RenderPassManager.hpp"
#include "ResourceManager.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <entt/entity/fwd.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace MEngine::Function::System
{
class MRenderSystem final : public MSystem
{
  private:
    std::shared_ptr<VulkanContext> mVulkanContext;
    std::shared_ptr<IMPipelineManager> mPipelineManager;
    std::shared_ptr<RenderPassManager> mRenderPassManager;

  private:
    uint32_t mFrameCount{1};
    uint32_t mCurrentFrameIndex{0};
    std::vector<vk::UniqueCommandBuffer> mGraphicsCommandBuffers;
    std::vector<vk::UniqueFence> mInFlightFences;
    std::vector<vk::Semaphore> mImageAvailableSemaphores;
    std::vector<vk::UniqueSemaphore> mRenderFinishedSemaphores;
    std::unordered_map<std::shared_ptr<MPipeline>, std::vector<entt::entity>> mRenderQueue;
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

    entt::entity mMainCameraEntity{};
    std::vector<vk::UniqueDescriptorSet> mGlobalDescriptorSets;
    vk::Buffer mCameraUBO;
    VmaAllocation mCameraUBOAllocation;
    VmaAllocationInfo mCameraUBOAllocationInfo;
    vk::Buffer mLightUBO;
    VmaAllocation mLightUBOAllocation;
    VmaAllocationInfo mLightUBOAllocationInfo;
    struct CameraParameters
    {
        glm::mat4 ProjectionMatrix = glm::identity<glm::mat4>();
        glm::mat4 ViewMatrix = glm::identity<glm::mat4>();
    } mCameraParameters{};

  public:
    MRenderSystem(std::shared_ptr<VulkanContext> context, std::shared_ptr<entt::registry> registry,
                  std::shared_ptr<ResourceManager> resourceManager,
                  std::shared_ptr<RenderPassManager> renderPassManager,
                  std::shared_ptr<IMPipelineManager> pipelineManager)
        : MSystem(registry, resourceManager), mVulkanContext(context), mRenderPassManager(renderPassManager),
          mPipelineManager(pipelineManager)
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
                                    vk::Pipeline pipeline, vk::DescriptorSet globalDescriptorSet,
                                    const std::vector<entt::entity> &entities);
    void End(vk::CommandBuffer commandBuffer, vk::Fence fence, vk::Semaphore signalSemaphore,
             vk::Semaphore waitSemaphore);
    void WriteGlobalDescriptorSet(uint32_t globalDescriptorSetIndex);
    // void RenderPostProcessPass();
};
} // namespace MEngine::Function::System