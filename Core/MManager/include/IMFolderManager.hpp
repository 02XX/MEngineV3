#pragma once
#include "IMManager.hpp"
#include "MFolder.hpp"
#include <memory>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class IMFolderManager : public virtual IMManager<MFolder>
{
  public:
    ~IMFolderManager() override = default;
    virtual std::shared_ptr<MFolder> Create(const std::string &name, const MFolderSetting &setting) = 0;
};
} // namespace MEngine::Core::Manager