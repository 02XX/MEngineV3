#pragma once
#include "IMManager.hpp"
#include "IUUIDGenerator.hpp"
#include "UUID.hpp"
#include "VulkanContext.hpp"
#include <concepts>
#include <memory>
#include <unordered_map>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
template <std::derived_from<MAsset> TAsset, std::derived_from<MAssetSetting> TSetting>
class MManager : public virtual IMManager<TAsset, TSetting>
{
  protected:
    // ID
    std::shared_ptr<VulkanContext> mVulkanContext;
    std::shared_ptr<IUUIDGenerator> mUUIDGenerator;

  public:
    MManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
        : mVulkanContext(vulkanContext), mUUIDGenerator(uuidGenerator)
    {
    }
    ~MManager() override = default;
};
} // namespace MEngine::Core::Manager