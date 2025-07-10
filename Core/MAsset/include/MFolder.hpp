#pragma once
#include "MAsset.hpp"
#include "MManager_fwd.hpp"
#include "UUID.hpp"
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <vector>
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
    UUID mParentFolderID;
    std::vector<UUID> mChildrenIDs;
    // 导航属性
    std::shared_ptr<MFolder> mParentFolder;
    std::vector<std::shared_ptr<MAsset>> mChildren;

  public:
    MFolder(const UUID &id, const std::string &name, const UUID &parentFolderID, const std::vector<UUID> &childrenIDs,
            const MFolderSetting &setting)
        : MAsset(id, name), mSetting(setting), mParentFolderID(parentFolderID), mChildrenIDs(childrenIDs)
    {
        mType = MAssetType::Folder;
        mState = MAssetState::Unloaded;
    }

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