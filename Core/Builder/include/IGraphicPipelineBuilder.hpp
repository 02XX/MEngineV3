#pragma once
#include "GraphicPipeline.hpp"
#include "IBuilder.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class IGraphicPipelineBuilder : public virtual IBuilder<GraphicPipeline>
{
  public:
    ~IGraphicPipelineBuilder() override = default;
    virtual void SetName(const std::string &name) = 0;
    virtual void SetVertexInputState() = 0;
    virtual void SetInputAssemblyState() = 0;
    virtual void SetShader() = 0;
    virtual void SetRasterizationState() = 0;
    virtual void SetViewportState() = 0;
    virtual void SetMultiSampleState() = 0;
    virtual void SetDepthStencilState() = 0;
    virtual void SetColorBlendState() = 0;
    virtual void SetLayout() = 0;
    virtual void SetRenderPass() = 0;
    virtual void SetDynamicState() = 0;
};
} // namespace MEngine::Core