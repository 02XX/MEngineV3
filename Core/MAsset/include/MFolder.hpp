#pragma once
#include "MAsset.hpp"
#include "MFolderSetting.hpp"
#include "MManager_fwd.hpp"
#include <nlohmann/json_fwd.hpp>
namespace MEngine::Core::Asset

{
class MFolder : public MAsset
{
    friend class nlohmann::adl_serializer<MFolder>;
    friend class Manager::MFolderManager;

  private:
    MFolderSetting mSetting;

  private:
    MFolder(const UUID &id, const MFolderSetting &setting) : MAsset(id), mSetting(setting)
    {
        mType = MAssetType::Folder;
        mState = MAssetState::Unloaded;
    }

  public:
    ~MFolder() override = default;
};

} // namespace MEngine::Core::Asset