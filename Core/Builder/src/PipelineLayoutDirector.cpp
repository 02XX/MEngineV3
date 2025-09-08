#include "PipelineLayoutDirector.hpp"

namespace MEngine::Core
{
std::unique_ptr<PipelineLayout> PipelineLayoutDirector::Make(IPipelineLayoutBuilder &builder)
{
    builder.Reset();
    builder.SetBindings();
    builder.SetLayout();
    builder.SetPushConstants();
    return builder.Build();
}
} // namespace MEngine::Core