#include "MFolderManager.hpp"
#include "Logger.hpp"
#include <memory>

namespace MEngine::Core::Manager
{
std::shared_ptr<MFolder> MFolderManager::Create(const std::string &name, const MFolderSetting &setting)
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
