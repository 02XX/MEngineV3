#pragma once
#include "IManager.hpp"
#include "PipelineLayout.hpp"
#include "PipelineLayoutType.hpp"
#include <vulkan/vulkan.hpp>

namespace MEngine::Core
{
class IPipelineLayoutManager : public virtual IManager<PipelineLayout>
{
  public:
    ~IPipelineLayoutManager() override = default;
    virtual std::shared_ptr<PipelineLayout> GetByType(PipelineLayoutType type) const = 0;
};
} // namespace MEngine::Core