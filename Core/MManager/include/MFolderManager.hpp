#pragma once
#include "IMFolderManager.hpp"
#include "IUUIDGenerator.hpp"
#include "MFolder.hpp"

#include "MManager.hpp"
#include "VulkanContext.hpp"
#include <memory>
#include <unordered_map>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{
class MFolderManager final : public MManager<MFolder>, public IMFolderManager
{
  public:
    MFolderManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
        : MManager(vulkanContext, uuidGenerator)
    {
    }
    ~MFolderManager() override = default;
    std::shared_ptr<MFolder> Create(const std::string &name, const MFolderSetting &setting) override;
    void Update(std::shared_ptr<MFolder> folder) override
    {
    }
    void CreateDefault() override;
    virtual void CreateVulkanResources(std::shared_ptr<MFolder> asset) override;
};
} // namespace MEngine::Core::Manager