#pragma once
#include "IMManager.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"
#include "MAssetSetting.hpp"
#include "UUID.hpp"
#include "VulkanContext.hpp"
#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>

using namespace MEngine::Core::Asset;
using namespace MEngine::Core::Manager;
namespace MEngine
{
namespace Resource
{
class ResourceManager final
{
  private:
    // DI
    std::shared_ptr<VulkanContext> mVulkanContext;

  private:
    std::unordered_map<UUID, std::shared_ptr<MAsset>> mAssets;
    std::unordered_map<std::type_index, std::shared_ptr<IMManagerBase>> mManagers;

  public:
    ResourceManager(std::shared_ptr<VulkanContext> vulkanContext) : mVulkanContext(vulkanContext)
    {
    }
    template <std::derived_from<MAsset> TAsset, std::derived_from<MAssetSetting> TSetting>
    void RegisterManager(std::shared_ptr<IMManager<TAsset, TSetting>> manager)
    {
        auto typeIndex = std::type_index(typeid(TAsset));
        if (mManagers.contains(typeIndex))
        {
            LogWarn("Manager for type {} already registered.", typeIndex.name());
            return;
        }
        mManagers[typeIndex] = manager;
        LogInfo("Registered manager for type: {}", typeIndex.name());
    }
    template <std::derived_from<MAsset> TAsset> std::shared_ptr<IMManager<TAsset, MAssetSetting>> GetManager() const
    {
        if (mManagers.contains(std::type_index(typeid(TAsset))))
        {
            return std::static_pointer_cast<IMManager<TAsset, MAssetSetting>>(
                mManagers.at(std::type_index(typeid(TAsset))));
        }
        return nullptr;
    }
    template <std::derived_from<MAsset> TAsset, std::derived_from<MAssetSetting> TSetting>
    std::shared_ptr<TAsset> CreateAsset(const TSetting &setting)
    {
        auto manager = std::static_pointer_cast<IMManager<TAsset, TSetting>>(mManagers.at(typeid(TAsset)));
        if (!manager)
        {
            throw std::runtime_error("Manager for asset type " + std::string(typeid(TAsset).name()) + " not found.");
        }
        auto asset = manager->Create(setting);
        mAssets[asset->GetID()] = asset;
        return asset;
    }
    std::shared_ptr<MAsset> GetAsset(const UUID &id) const;
    template <std::derived_from<MAsset> TAsset> std::shared_ptr<TAsset> GetAsset(const UUID &id) const
    {
        return std::static_pointer_cast<TAsset>(GetAsset(id));
    }
    void UpdateAsset(const UUID &id);
    void UpdateAsset(std::shared_ptr<MAsset> asset);
    void DeleteAsset(const UUID &id);
};
} // namespace Resource
} // namespace MEngine