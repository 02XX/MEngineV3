#pragma once
#include "Pipeline.hpp"
namespace MEngine::Core
{
class GraphicPipeline final : public Pipeline
{
  private:
    // TODO: renderpass持久化资源化可配置
    uint32_t mSubPass{0};

  protected:
    GraphicPipeline() : Pipeline()
    {
        mType = AssetType::Shader;
    }

  public:
    GraphicPipeline(const UUID &id, const std::string &name, vk::UniquePipeline pipeline,
                    PipelineLayoutType pipelineLayoutType, uint32_t subPass)
        : Pipeline(id, name, std::move(pipeline), pipelineLayoutType), mSubPass(subPass)
    {
        mType = AssetType::Shader;
    }
    ~GraphicPipeline() override = default;
};
} // namespace MEngine::Core