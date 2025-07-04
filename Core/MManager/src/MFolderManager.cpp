#include "MFolderManager.hpp"
#include <memory>

namespace MEngine::Core::Manager
{
std::shared_ptr<MFolder> MFolderManager::Create(const MFolderSetting &setting)
{
    auto folder = std::shared_ptr<MFolder>(new MFolder(mUUIDGenerator->Create(), setting));
    return folder;
}
} // namespace MEngine::Core::Manager
