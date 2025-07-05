#include "ResourceManager.hpp"
#include "Logger.hpp"
#include "MAsset.hpp"

namespace MEngine::Resource
{
std::shared_ptr<MAsset> ResourceManager::GetAsset(const UUID &id) const
{
    if (mAssets.contains(id))
    {
        return mAssets.at(id);
    }
    return nullptr;
}
void ResourceManager::UpdateAsset(const UUID &id)
{
    if (auto asset = GetAsset(id))
    {
        UpdateAsset(asset);
    }
    else
    {
        LogError("Asset with ID {} does not exist.", id.ToString());
    }
}
void ResourceManager::UpdateAsset(std::shared_ptr<MAsset> asset)
{
    // 分发至各自的manager
    switch (asset->GetType())
    {
    case Core::Asset::MAssetType::Texture:
    case Core::Asset::MAssetType::Material:
    case Core::Asset::MAssetType::Mesh:
    case Core::Asset::MAssetType::Shader:
    case Core::Asset::MAssetType::Animation:
    case Core::Asset::MAssetType::Scene:
    case Core::Asset::MAssetType::Audio:
    case Core::Asset::MAssetType::Font:
    case Core::Asset::MAssetType::Folder:
    case Core::Asset::MAssetType::File:
    case Core::Asset::MAssetType::Script:
    case Core::Asset::MAssetType::Unknown:
        break;
    }
}
void ResourceManager::DeleteAsset(const UUID &id)
{
    if (mAssets.contains(id))
    {
        mAssets.erase(id);
    }
    else
    {
        LogError("Asset with ID {} does not exist.", id.ToString());
    }
}
} // namespace MEngine::Resource