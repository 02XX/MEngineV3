#pragma once
#include "IPipelineLayoutBuilder.hpp"
#include "VulkanContext.hpp"
#include <vector>
#include <vulkan/vulkan_structs.hpp>
namespace MEngine::Core
{
class PipelineLayoutBuilder : public virtual IPipelineLayoutBuilder
{
  protected:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    std::unique_ptr<PipelineLayout> mPipelineLayout{};

  public:
    ~PipelineLayoutBuilder() override = default;
    PipelineLayoutBuilder(std::shared_ptr<VulkanContext> vulkanContext) : mVulkanContext(vulkanContext)
    {
        mPipelineLayout = std::unique_ptr<PipelineLayout>(new PipelineLayout());
    }
    std::unique_ptr<PipelineLayout> Build() override;
    void Reset() override;
    void SetBindings() override;
    void SetLayout() override;
    void SetPushConstants() override;
};
} // namespace MEngine::Core