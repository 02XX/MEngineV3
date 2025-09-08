#pragma once
#include "IPipelineLayoutManager.hpp"
#include "Manager.hpp"
#include <memory>
#include <string>

namespace MEngine::Core
{

class PipelineLayoutManager final : public Manager<PipelineLayout>, public IPipelineLayoutManager
{
  private:
    std::unordered_map<PipelineLayoutType, UUID> mTypeToID;

  public:
    PipelineLayoutManager(std::shared_ptr<VulkanContext> vulkanContext) : Manager<PipelineLayout>(vulkanContext)
    {
    }
    ~PipelineLayoutManager() override = default;
    void Add(std::shared_ptr<PipelineLayout> asset) override;
    std::shared_ptr<PipelineLayout> GetByType(PipelineLayoutType type) const override;
};
} // namespace MEngine::Core