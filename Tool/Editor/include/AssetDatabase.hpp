#pragma once
#include "IMFolderManager.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"
#include "MFolder.hpp"
#include "MTexture.hpp"
#include "ResourceManager.hpp"
#include "Serialize.hpp"
#include "UUID.hpp"
#include <concepts>
#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_map>

using namespace MEngine::Core;
using namespace MEngine::Core::Manager;
using namespace MEngine::Resource;
namespace MEngine
{
namespace Editor
{
class AssetDatabase
{
  private:
    // DI
    std::shared_ptr<ResourceManager> mResourceManager;

  private:
    std::unordered_map<std::filesystem::path, UUID> mPath2UUID;

  public:
    AssetDatabase(std::shared_ptr<ResourceManager> resourceManager) : mResourceManager(resourceManager)
    {
    }
    //  Read
    std::shared_ptr<MAsset> GetAsset(const std::filesystem::path &path) const;
    template <std::derived_from<MAsset> TAsset>
    std::shared_ptr<TAsset> GetAsset(const std::filesystem::path &path) const
    {
        if (mPath2UUID.contains(path))
        {
            return mResourceManager->GetAsset<TAsset>(mPath2UUID.at(path));
        }
        LogWarn("Path {} not found in AssetDatabase.", path.string());
        return nullptr;
    }
    //  Update
    void UpdateAsset(const std::filesystem::path &path);
    //  Delete
    void DeleteAsset(const std::filesystem::path &path);

    //  持久化
    std::shared_ptr<MAsset> LoadAsset(const std::filesystem::path &path);
    template <std::derived_from<MAsset> TAsset> std::shared_ptr<TAsset> LoadAsset(const std::filesystem::path &path)
    {
        auto asset = LoadAsset(path);
        return std::static_pointer_cast<TAsset>(asset);
    }
    template <std::derived_from<MAsset> TAsset>
    void SaveAsset(std::shared_ptr<TAsset> asset, const std::filesystem::path &path)
    {
        auto savePath = path;
        if (savePath.has_extension())
        {
            savePath.replace_extension(".masset");
        }
        else
        {
            savePath += ".masset";
        }
        auto uniquePath = GenerateUniqueAssetPath(savePath);
        std::ofstream file(uniquePath);
        if (!file.is_open())
        {
            throw std::runtime_error("Failed to open asset file for writing: " + uniquePath.string());
        }
        asset->SetPath(uniquePath);
        asset->SetName(uniquePath.filename().stem().string());
        json j;
        j = *asset;
        file << j.dump(4);
        file.close();
        auto folderManager = mResourceManager->GetManager<MFolder, IMFolderManager>();
        auto parentFolder = folderManager->Get(mPath2UUID[uniquePath.parent_path()]);
        if (parentFolder != nullptr)
        {
            parentFolder->AddChild(asset);
        }
        mPath2UUID[asset->GetPath()] = asset->GetID();
    }

    std::shared_ptr<MFolder> CreateFolder(const std::filesystem::path &parentPath,
                                          const std::string &newFolderName = "New Folder",
                                          const MFolderSetting &setting = {});
    void DeleteFolder(const std::filesystem::path &directory);
    std::shared_ptr<MFolder> LoadFolder(const std::filesystem::path &directory);
    void SaveFolder(std::shared_ptr<MFolder> folder, const std::filesystem::path &directory);
    // Operations
    std::filesystem::path GenerateUniqueAssetPath(std::filesystem::path path);
    void MoveAsset(const std::filesystem::path &srcPath, const std::filesystem::path &dstPath);
    void RenameAsset(const std::filesystem::path &path, const std::string &newName);
    void CopyAsset(const std::filesystem::path &srcPath, const std::filesystem::path &dstPath);

    std::shared_ptr<MFolder> LoadDatabase(const std::filesystem::path &directory,
                                          const std::filesystem::path &parentDirectory = {});
    // External Assets
    std::shared_ptr<MModel> LoadFBX(const std::filesystem::path &path);
    std::shared_ptr<MTexture> LoadPNG(const std::filesystem::path &path);
};
} // namespace Editor
} // namespace MEngine