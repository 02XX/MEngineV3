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
    ~ComputePipeline() override = default;
};
} // namespace MEngine::Core