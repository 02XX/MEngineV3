#pragma once
#include "IMMeshManager.hpp"
#include "IMModelManager.hpp"
#include "IMPBRMaterialManager.hpp"
#include "MManager.hpp"
#include <memory>
#include <unordered_map>
#include <vulkan/vulkan_handles.hpp>

using namespace MEngine::Core::Asset;
namespace MEngine::Core::Manager
{

class MModelManager final : public MManager<MModel>, public IMModelManager
{
  private:
    std::shared_ptr<IMMeshManager> mMeshManager;
    std::shared_ptr<IMPBRMaterialManager> mMaterialManager;

  public:
    MModelManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                  std::shared_ptr<IMMeshManager> meshManager, std::shared_ptr<IMPBRMaterialManager> materialManager)
        : MManager<MModel>(vulkanContext, uuidGenerator), mMeshManager(meshManager), mMaterialManager(materialManager)
    {
        CreateDefault();
    }
    ~MModelManager() override = default;
    std::shared_ptr<MModel> Create(const std::string &name, const std::vector<UUID> &meshIDs,
                                   const std::vector<UUID> &materialIDs, std::unique_ptr<Node> rootNode,
                                   const MModelSetting &setting) override;
    void Update(std::shared_ptr<MModel> model) override;
    std::shared_ptr<MModel> CreateCube() override;
    std::shared_ptr<MModel> CreateSphere() override;
    std::shared_ptr<MModel> CreatePlane() override;
    std::shared_ptr<MModel> CreateCylinder() override;
    void CreateDefault() override;
    virtual void CreateVulkanResources(std::shared_ptr<MModel> asset) override;
};
} // namespace MEngine::Core::Manager