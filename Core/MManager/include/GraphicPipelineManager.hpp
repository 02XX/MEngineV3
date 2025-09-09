#pragma once
#include "IGraphicPipelineManager.hpp"
#include "Manager.hpp"
#include <unordered_map>
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class GraphicPipelineManager final : public Manager<GraphicPipeline>, public virtual IGraphicPipelineManager
{
  private:
    std::unordered_map<std::string, UUID> mNameToID;

  public:
    ~GraphicPipelineManager() override = default;
    void Add(std::shared_ptr<GraphicPipeline> asset) override;
    std::shared_ptr<GraphicPipeline> GetByName(const std::string &name) const override;
};
} // namespace MEngine::Core