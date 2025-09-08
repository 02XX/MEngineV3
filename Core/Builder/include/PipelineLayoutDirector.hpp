#pragma once
#include "IPipelineLayoutBuilder.hpp"

namespace MEngine::Core
{

class PipelineLayoutDirector
{
  public:
    std::unique_ptr<PipelineLayout> Make(IPipelineLayoutBuilder &builder);
};
} // namespace MEngine::Core