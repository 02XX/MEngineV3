#include "MFolderManager.hpp"
#include <memory>

namespace MEngine::Core::Manager
{
std::shared_ptr<MFolder> MFolderManager::Create(const MFolderSetting &setting, const std::string &name)
{
    auto folder = std::make_shared<MFolder>(mUUIDGenerator->Create(), name, setting);
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
