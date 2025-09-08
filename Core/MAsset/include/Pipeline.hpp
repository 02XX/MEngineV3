#pragma once
#include "Asset.hpp"
#include "Pipeline.hpp"
#include "PipelineLayout.hpp"
#include "PipelineLayoutType.hpp"
#include "UUID.hpp"
#include <memory>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class Pipeline : public Asset
{
  private:
    vk::UniquePipeline mPipeline{nullptr};
    PipelineLayoutType mPipelineLayoutType{PipelineLayoutType::None};

  protected:
    Pipeline() : Asset(UUID{}, "Unnamed")
    {
    }

  public:
    Pipeline(const UUID &id, const std::string &name, vk::UniquePipeline pipeline,
             PipelineLayoutType pipelineLayoutType)
        : Asset(id, name), mPipeline(std::move(pipeline)), mPipelineLayoutType(pipelineLayoutType)
    {
    }
    ~Pipeline() override = default;
    inline vk::Pipeline GetPipeline() const
    {
        return mPipeline.get();
    }
};
} // namespace MEngine::Core