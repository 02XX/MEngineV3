#pragma once
#include "Builder.hpp"
#include "GraphicPipeline.hpp"
#include "IGraphicPipelineBuilder.hpp"
#include "IPipelineLayoutManager.hpp"
#include "IShaderManager.hpp"
#include "RenderPassManager.hpp"
#include "VulkanContext.hpp"
#include <memory>

namespace MEngine::Core
{
class GraphicPipelineBuilder : public Builder<GraphicPipeline>, public virtual IGraphicPipelineBuilder
{

  protected:
    std::shared_ptr<VulkanContext> mVulkanContext{};
    std::shared_ptr<IShaderManager> mShaderManager{};
    std::shared_ptr<RenderPassManager> mRenderPassManager{};
    std::shared_ptr<IPipelineLayoutManager> mPipelineLayoutManager{};

  protected:
    std::unique_ptr<GraphicPipeline> mGraphicPipeline;

  public:
    GraphicPipelineBuilder(std::shared_ptr<VulkanContext> vulkanContext,
                           std::shared_ptr<IPipelineLayoutManager> pipelineLayoutManager,
                           std::shared_ptr<IShaderManager> shaderManager,
                           std::shared_ptr<RenderPassManager> renderPassManager)
        : mVulkanContext(vulkanContext), mShaderManager(shaderManager), mRenderPassManager(renderPassManager),
          mPipelineLayoutManager(pipelineLayoutManager)
    {
        mGraphicPipeline = std::unique_ptr<GraphicPipeline>(new GraphicPipeline());
    }
    ~GraphicPipelineBuilder() override = default;
    virtual void Reset() override;
    virtual std::unique_ptr<GraphicPipeline> Build() override;
    virtual void SetVertexInputState() override;
    virtual void SetInputAssemblyState() override;
    virtual void SetRasterizationState() override;
    virtual void SetViewportState() override;
    virtual void SetMultiSampleState() override;
    virtual void SetDepthStencilState() override;
    virtual void SetColorBlendState() override;
    virtual void SetDynamicState() override;
};
} // namespace MEngine::Core