#pragma once
#include "MPipeline.hpp"
#include "VulkanContext.hpp"
#include <cstdint>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>
using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class RenderPassManager final
{
  private:
    // DI
    std::shared_ptr<VulkanContext> mVulkanContext;

  private:
    std::unordered_map<RenderPassType, uint32_t> mSubPasses;
    vk::UniqueRenderPass mCompositionRenderPass;

  private:
    void CreateCompositionRenderPass();
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
        CreateCompositionRenderPass();
    }
    std::tuple<vk::RenderPass, uint32_t> GetRenderPass(RenderPassType type) const;
    inline vk::RenderPass GetCompositionRenderPass() const
    {
        return mCompositionRenderPass.get();
    }
};

} // namespace MEngine::Core::Manager