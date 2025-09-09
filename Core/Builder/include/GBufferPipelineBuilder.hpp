#pragma once
#include "GraphicPipelineBuilder.hpp"

namespace MEngine::Core
{
class GBufferPipelineBuilder : public GraphicPipelineBuilder, public virtual IGraphicPipelineBuilder
{
  public:
    GBufferPipelineBuilder(std::shared_ptr<VulkanContext> vulkanContext,
                           std::shared_ptr<IPipelineLayoutManager> pipelineLayoutManager,
                           std::shared_ptr<IShaderManager> shaderManager,
                           std::shared_ptr<RenderPassManager> renderPassManager)
        : GraphicPipelineBuilder(vulkanContext, pipelineLayoutManager, shaderManager, renderPassManager)
    {
    }
    ~GBufferPipelineBuilder() override = default;
    virtual void SetName() override;
    virtual void SetShader() override;
    virtual void SetColorBlendState() override;
    virtual void SetLayout() override;
    virtual void SetRenderPass() override;
};
} // namespace MEngine::Core