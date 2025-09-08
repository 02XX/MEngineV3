#pragma once
#include "IPipelineLayoutBuilder.hpp"
#include "PipelineLayoutBuilder.hpp"
namespace MEngine::Core
{
class PBRPipelineLayoutBuilder : public PipelineLayoutBuilder, public virtual IPipelineLayoutBuilder
{
  private:
    // 绑定信息
    std::vector<vk::DescriptorSetLayoutBinding> mPBRPipelineLayoutBindings{};
    vk::UniqueDescriptorSetLayout mPBRPipelineLayoutDescriptorSetLayout{};

  public:
    PBRPipelineLayoutBuilder(std::shared_ptr<VulkanContext> vulkanContext) : PipelineLayoutBuilder(vulkanContext)
    {
    }
    ~PBRPipelineLayoutBuilder() override = default;

    virtual void Reset() override;
    virtual std::unique_ptr<PipelineLayout> Build() override;
    virtual void SetBindings() override;
    virtual void SetLayout() override;
    virtual void SetPushConstants() override;
};
} // namespace MEngine::Core