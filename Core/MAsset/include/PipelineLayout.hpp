#pragma once
#include "Asset.hpp"
#include "PipelineLayoutType.hpp"
#include "UUID.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class PipelineLayout : public Asset
{
  private:
    vk::UniquePipelineLayout mPipelineLayout{nullptr};
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout{nullptr};
    PipelineLayoutType mPipelineLayoutType{PipelineLayoutType::None};

  protected:
    PipelineLayout() : Asset(UUID{}, "Unnamed")
    {
    }

  public:
    PipelineLayout(const UUID &id, const std::string &name, PipelineLayoutType pipelineLayoutType,
                   vk::UniquePipelineLayout pipelineLayout)
        : Asset(id, name), mPipelineLayoutType(pipelineLayoutType), mPipelineLayout(std::move(pipelineLayout))
    {
    }
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