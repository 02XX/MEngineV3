#pragma once
#include "IPipelineLayoutBuilder.hpp"
#include "PipelineLayoutBuilder.hpp"
namespace MEngine::Core
{
class PBRPipelineLayoutBuilder : public PipelineLayoutBuilder, public virtual IPipelineLayoutBuilder
{
  public:
    PBRPipelineLayoutBuilder(std::shared_ptr<VulkanContext> vulkanContext) : PipelineLayoutBuilder(vulkanContext)
    {
    }
    ~PBRPipelineLayoutBuilder() override = default;

    virtual void SetBindings() override;
    virtual void SetLayout() override;
    virtual void SetPushConstants() override;
    virtual void SetPipelineLayoutType() override;
};
} // namespace MEngine::Core