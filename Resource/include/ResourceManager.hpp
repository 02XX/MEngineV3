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
        RegisterManager<MTexture>(mTextureManager);
        RegisterManager<MFolder>(mFolderManager);
        RegisterManager<MPipeline>(mPipelineManager);
        RegisterManager<MMesh>(mMeshManager);
        RegisterManager<MModel>(mModelManager);
        RegisterManager<MPBRMaterial>(mPBRMaterialManager);
        CreateDefaultAssets();
    }
    template <std::derived_from<MAsset> TAsset> void RegisterManager(std::shared_ptr<IMManager<TAsset>> manager)
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
    template <std::derived_from<MAsset> TAsset, std::derived_from<IMManager<TAsset>> TManager>
    std::shared_ptr<TManager> GetManager() const
    {
        if (mManagers.contains(std::type_index(typeid(TAsset))))
        {
            return std::dynamic_pointer_cast<TManager>(mManagers.at(std::type_index(typeid(TAsset))));
        }
        return nullptr;
    }
    
    template <std::derived_from<MAsset> TAsset> std::shared_ptr<TAsset> GetAsset(const UUID &id) const
    {
        auto typeIndex = std::type_index(typeid(TAsset));
        auto manager = std::static_pointer_cast<IMManager<TAsset>>(mManagers.at(typeIndex));
        return manager->Get(id);
    }
    void UpdateAsset(const UUID &id);
    void DeleteAsset(const UUID &id);
    void CreateDefaultAssets();
};
} // namespace Resource
} // namespace MEngine