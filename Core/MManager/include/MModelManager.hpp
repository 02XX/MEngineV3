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
        CreateDefault();
    }
    ~MModelManager() override = default;
    std::shared_ptr<MModel> Create(const MModelSetting &setting, const std::string &name = "New Model") override;
    void Update(std::shared_ptr<MModel> model) override
    {
    }
    std::shared_ptr<MModel> CreateCube() override;
    std::shared_ptr<MModel> CreateSphere() override;
    std::shared_ptr<MModel> CreatePlane() override;
    std::shared_ptr<MModel> CreateCylinder() override;
    void CreateDefault() override;
    virtual void CreateVulkanResources(std::shared_ptr<MModel> asset) override;
};
} // namespace MEngine::Core::Manager