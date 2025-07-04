#pragma once
#include "IMFolderManager.hpp"
#include "IUUIDGenerator.hpp"
#include "MFolder.hpp"
#include "MFolderSetting.hpp"
#include "MManager.hpp"
#include "VulkanContext.hpp"
#include <memory>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class MFolderManager final : public MManager<MFolder, MFolderSetting>, public IMFolderManager
{
  public:
    MFolderManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
        : MManager(vulkanContext, uuidGenerator)
    {
    }
    ~MFolderManager() override = default;
    std::shared_ptr<MFolder> Create(const MFolderSetting &setting) override;
};
} // namespace MEngine::Core::Manager