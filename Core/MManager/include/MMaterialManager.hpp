#pragma once
#include "IMMaterialManager.hpp"
#include "MManager.hpp"

namespace MEngine::Core::Manager
{
template <std::derived_from<MMaterial> TAsset, std::derived_from<MMaterialSetting> TSetting>
class MMaterialManager : public MManager<TAsset, TSetting>, public virtual IMMaterialManager<TAsset, TSetting>
{
  public:
    MMaterialManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
        : MManager<TAsset, TSetting>(vulkanContext, uuidGenerator)
    {
    }
    ~MMaterialManager() override = default;
};
} // namespace MEngine::Core::Manager