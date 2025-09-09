#pragma once
#include "IBuilder.hpp"
#include "PipelineLayout.hpp"

namespace MEngine::Core
{
class IPipelineLayoutBuilder : public virtual IBuilder<PipelineLayout>
{
  public:
    ~IPipelineLayoutBuilder() override = default;
    virtual void SetBindings() = 0;
    virtual void SetLayout() = 0;
    virtual void SetPushConstants() = 0;
    virtual void SetPipelineLayoutType() = 0;
};
} // namespace MEngine::Core