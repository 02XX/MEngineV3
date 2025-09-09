#pragma once
#include "Asset.hpp"
#include "PipelineLayoutType.hpp"
#include "UUID.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class PipelineLayout : public Asset
{
    friend class PipelineLayoutBuilder;
    friend class PBRPipelineLayoutBuilder;

  private:
    vk::UniquePipelineLayout mPipelineLayout{nullptr};
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout{nullptr};

  protected:
    PipelineLayoutType mPipelineLayoutType{PipelineLayoutType::None};
    vk::PipelineLayoutCreateInfo mPipelineLayoutCreateInfo{};
    std::vector<vk::DescriptorSetLayout> mSetLayouts{};
    std::vector<vk::PushConstantRange> mPushConstantRanges{};

    std::vector<std::vector<vk::DescriptorSetLayoutBinding>> mPipelineLayoutBindings{};
    std::vector<vk::UniqueDescriptorSetLayout> mPipelineLayoutDescriptorSetLayouts{};

  protected:
    PipelineLayout() : Asset()
    {
    }

  public:
    ~PipelineLayout() override = default;
    inline vk::PipelineLayout GetPipelineLayout() const
    {
        return mPipelineLayout.get();
    }
    inline PipelineLayoutType GetPipelineLayoutType() const
    {
        return mPipelineLayoutType;
    }
};
} // namespace MEngine::Core