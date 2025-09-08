#include "GraphicPipelineDirector.hpp"
#include "Pipeline.hpp"
#include <memory>

namespace MEngine::Core
{
std::unique_ptr<GraphicPipeline> GraphicPipelineDirector::Make(IGraphicPipelineBuilder &builder)
{
    builder.SetVertexInputState();
    builder.SetInputAssemblyState();
    builder.SetShader();
    builder.SetRasterizationState();
    builder.SetViewportState();
    builder.SetMultiSampleState();
    builder.SetDepthStencilState();
    builder.SetColorBlendState();
    builder.SetLayout();
    builder.SetRenderPass();
    builder.SetDynamicState();
    return builder.Build();
}
} // namespace MEngine::Core