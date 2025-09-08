#pragma once
#include "IGraphicPipelineBuilder.hpp"

namespace MEngine::Core
{

class GraphicPipelineDirector
{
  public:
    std::unique_ptr<GraphicPipeline> Make(IGraphicPipelineBuilder &builder);
};
} // namespace MEngine::Core