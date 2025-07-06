#pragma once
#include "IMManager.hpp"
#include "MFolder.hpp"

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMFolderManager : public IMManager<MFolder, MFolderSetting>
{
  public:
    ~IMFolderManager() override = default;
};
} // namespace MEngine::Core::Manager