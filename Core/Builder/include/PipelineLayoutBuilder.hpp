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
    std::shared_ptr<VulkanContext> mVulkanContext;

  protected:
    vk::PipelineLayoutCreateInfo mPipelineLayoutCreateInfo{};
    std::vector<vk::DescriptorSetLayout> mSetLayouts{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

    std::vector<vk::DescriptorSetLayoutBinding> mPipelineLayoutBindings{};
    vk::UniqueDescriptorSetLayout mPipelineLayoutDescriptorSetLayout{};

  public:
    ~PipelineLayoutBuilder() override = default;
    PipelineLayoutBuilder(std::shared_ptr<VulkanContext> vulkanContext) : mVulkanContext(vulkanContext)
    {
    }
    void Reset() override;
    void SetBindings() override;
    void SetLayout() override;
    void SetPushConstants() override;
};
} // namespace MEngine::Core