#pragma once
#include "IMMeshManager.hpp"
#include "IMModelManager.hpp"
#include "IMPBRMaterialManager.hpp"
#include "MManager.hpp"
#include <memory>
#include <vulkan/vulkan_handles.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

class MModelManager final : public MManager<MModel, MModelSetting>, public IMModelManager
{
  private:
    std::shared_ptr<IMMeshManager> mMeshManager;
    std::shared_ptr<IMPBRMaterialManager> mMaterialManager;

  public:
    MModelManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                  std::shared_ptr<IMMeshManager> meshManager, std::shared_ptr<IMPBRMaterialManager> materialManager)
        : MManager<MModel, MModelSetting>(vulkanContext, uuidGenerator), mMeshManager(meshManager),
          mMaterialManager(materialManager)
    {
    }
    ~MModelManager() override = default;
    std::shared_ptr<MModel> Create(const MModelSetting &setting) override;
    void Update(std::shared_ptr<MModel> model) override
    {
    }
    std::shared_ptr<MModel> CreateCube() override;
};
} // namespace MEngine::Core::Manager