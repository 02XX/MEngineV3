#include "MFolderManager.hpp"
#include "Logger.hpp"
#include <memory>

namespace MEngine::Core::Manager
{
std::shared_ptr<MFolder> MFolderManager::Create(const std::string &name, const UUID &parentFolderID,
                                                const std::vector<UUID> &childrenIDs, const MFolderSetting &setting)
{
    auto folder = std::make_shared<MFolder>(mUUIDGenerator->Create(), name, parentFolderID, childrenIDs, setting);
    // 设置导航属性
    folder->mParentFolder = Get(parentFolderID);
    if (folder->mParentFolder)
    {
        folder->mParentFolder->AddChild(folder);
        folder->SetPath(folder->mParentFolder->GetPath() / folder->GetName());
    }
    else
    {
        folder->SetPath(folder->GetName());
    }
    mAssets[folder->GetID()] = folder;
    return folder;
}
void MFolderManager::CreateDefault()
{
}
void MFolderManager::CreateVulkanResources(std::shared_ptr<MFolder> asset)
{
}

} // namespace MEngine::Core::Manager
