#include "PipelineLayoutDirector.hpp"

namespace MEngine::Core
{
std::unique_ptr<PipelineLayout> PipelineLayoutDirector::Make(IPipelineLayoutBuilder &builder)
{
    builder.SetBindings();
    builder.SetLayout();
    builder.SetPushConstants();
    builder.SetPipelineLayoutType();
    return builder.Build();
}
} // namespace MEngine::Core