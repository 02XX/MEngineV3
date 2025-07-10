#pragma once
#include "IMPBRMaterialManager.hpp"
#include "MMaterialManager.hpp"
namespace MEngine::Core::Manager
{
class MPBRMaterialManager final : public MMaterialManager<MPBRMaterial, MPBRMaterialSetting>,
                                  public IMPBRMaterialManager
{
  public:
    MPBRMaterialManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                        std::shared_ptr<IMPipelineManager> pipelineManager,
                        std::shared_ptr<IMTextureManager> textureManager)
        : MMaterialManager<MPBRMaterial, MPBRMaterialSetting>(vulkanContext, uuidGenerator, pipelineManager,
                                                              textureManager)
    {
        CreateDefault();
    }
    ~MPBRMaterialManager() override = default;
    std::shared_ptr<MPBRMaterial> Create(const MPBRMaterialSetting &setting,
                                         const std::string &name = "New PBR Material") override;
    void Update(std::shared_ptr<MPBRMaterial> material) override
    {
    }
    void Write(std::shared_ptr<MPBRMaterial> material) override;
    void CreateDefault() override;
    std::shared_ptr<MPBRMaterial> CreateDefaultMaterial() override;
    virtual void CreateVulkanResources(std::shared_ptr<MPBRMaterial> asset) override;
};
} // namespace MEngine::Core::Manager