#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include <memory>
#include <nlohmann/json_fwd.hpp>
namespace MEngine::Core::Asset
{

class MFolderSetting final : public MAssetSetting
{
  public:
    ~MFolderSetting() override = default;
};
class MFolder : public MAsset
{
    friend class nlohmann::adl_serializer<MFolder>;
    friend class Manager::MFolderManager;

  private:
    MFolderSetting mSetting;
    std::shared_ptr<MFolder> mParentFolder;
    std::vector<std::shared_ptr<MAsset>> mChildren;

  private:
    MFolder(const UUID &id, const MFolderSetting &setting) : MAsset(id), mSetting(setting)
    {
        mType = MAssetType::Folder;
        mState = MAssetState::Unloaded;
    }

  public:
    ~MFolder() override = default;
    inline std::shared_ptr<MFolder> GetParentFolder() const
    {
        return mParentFolder;
    }
    inline void SetParentFolder(const std::shared_ptr<MFolder> &parent)
    {
        mParentFolder = parent;
    }
    inline const std::vector<std::shared_ptr<MAsset>> &GetChildren() const
    {
        return mChildren;
    }
    inline void AddChild(const std::shared_ptr<MAsset> &child)
    {
        mChildren.push_back(child);
    }
    inline void RemoveChild(const std::shared_ptr<MAsset> &child)
    {
        mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), child), mChildren.end());
    }
};

} // namespace MEngine::Core::Asset