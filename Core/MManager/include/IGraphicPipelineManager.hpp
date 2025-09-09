#pragma once
#include "GraphicPipeline.hpp"
#include "IManager.hpp"
#include <vulkan/vulkan.hpp>


namespace MEngine::Core
{
class IGraphicPipelineManager : public virtual IManager<GraphicPipeline>
{
  public:
    ~IGraphicPipelineManager() override = default;
    virtual std::shared_ptr<GraphicPipeline> GetByName(const std::string &name) const = 0;
};
} // namespace MEngine::Core