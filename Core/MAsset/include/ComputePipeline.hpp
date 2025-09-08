#pragma once
#include "Pipeline.hpp"
namespace MEngine::Core
{
class ComputePipeline final : public Pipeline
{
  protected:
    ComputePipeline() : Pipeline()
    {
        mType = AssetType::Shader;
    }

  public:
    ComputePipeline(const UUID &id, const std::string &name, vk::UniquePipeline pipeline,
                    PipelineLayoutType pipelineLayoutType)
        : Pipeline(id, name, std::move(pipeline), pipelineLayoutType)
    {
        mType = AssetType::Shader;
    }
    ~ComputePipeline() override = default;
};
} // namespace MEngine::Core