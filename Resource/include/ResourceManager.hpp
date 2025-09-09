#pragma once

using namespace MEngine::Core;
namespace MEngine::Resource
{

class ResourceManager final
{
  public:
    virtual ~ResourceManager() = default;
    virtual std::filesystem::path GetAssetPath(const UUID &id) const = 0;
    virtual void LoadAsset(std::filesystem::path path) = 0;
    template <std::derived_from<Asset> TAsset> void SaveAsset(std::shared_ptr<TAsset> asset, std::filesystem::path path)
    {
    }
};
} // namespace MEngine::Resource