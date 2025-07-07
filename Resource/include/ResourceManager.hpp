#pragma once
#include "IMManager.hpp"
#include "IMPBRMaterialManager.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"

#include "MTexture.hpp"
#include "UUID.hpp"
#include "VulkanContext.hpp"
#include <concepts>
#include <memory>
#include <typeindex>
#include <unordered_map>

#include "IMFolderManager.hpp"
#include "IMMeshManager.hpp"
#include "IMModelManager.hpp"
#include "IMPBRMaterialManager.hpp"
#include "IMPipelineManager.hpp"
#include "IMTextureManager.hpp"

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
    // Managers
    std::shared_ptr<IMMeshManager> mMeshManager;
    std::shared_ptr<IMModelManager> mModelManager;
    std::shared_ptr<IMPBRMaterialManager> mPBRMaterialManager;
    std::shared_ptr<IMTextureManager> mTextureManager;
    std::shared_ptr<IMFolderManager> mFolderManager;
    std::shared_ptr<IMPipelineManager> mPipelineManager;

  private:
    std::unordered_map<std::type_index, std::unordered_map<UUID, std::shared_ptr<MAsset>>> mAssets;
    std::unordered_map<std::type_index, std::shared_ptr<IMManagerBase>> mManagers;

  public:
    ResourceManager(std::shared_ptr<VulkanContext> vulkanContext,

                    std::shared_ptr<IMMeshManager> meshManager, std::shared_ptr<IMModelManager> modelManager,
                    std::shared_ptr<IMPipelineManager> pipelineManager,
                    std::shared_ptr<IMPBRMaterialManager> pbrMaterialManager,
                    std::shared_ptr<IMTextureManager> textureManager, std::shared_ptr<IMFolderManager> folderManager

                    )
        : mVulkanContext(vulkanContext), mMeshManager(meshManager), mModelManager(modelManager),
          mPipelineManager(pipelineManager), mPBRMaterialManager(pbrMaterialManager), mTextureManager(textureManager),
          mFolderManager(folderManager)
    {
        RegisterManager<MTexture, MTextureSetting>(mTextureManager);
        RegisterManager<MFolder, MFolderSetting>(mFolderManager);
        RegisterManager<MPipeline, MPipelineSetting>(mPipelineManager);
        RegisterManager<MMesh, MMeshSetting>(mMeshManager);
        RegisterManager<MModel, MModelSetting>(mModelManager);
        RegisterManager<MPBRMaterial, MPBRMaterialSetting>(mPBRMaterialManager);
        CreateDefaultAssets();
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
            LogError("Manager for asset type {} not found.", typeid(TAsset).name());
            throw std::runtime_error("Manager for asset type " + std::string(typeid(TAsset).name()) + " not found.");
        }
        auto asset = manager->Create(setting);
        mAssets[typeid(TAsset)][asset->GetID()] = asset;
        return asset;
    }
    template <std::derived_from<MAsset> TAsset> std::shared_ptr<TAsset> GetAsset(const UUID &id) const
    {
        auto typeIndex = std::type_index(typeid(TAsset));
        if (mAssets.contains(typeIndex) && mAssets.at(typeIndex).contains(id))
        {
            return std::static_pointer_cast<TAsset>(mAssets.at(typeIndex).at(id));
        }
        return nullptr;
    }
    void UpdateAsset(const UUID &id);
    void DeleteAsset(const UUID &id);
    void CreateDefaultAssets();
};
} // namespace Resource
} // namespace MEngine