#pragma once
#include "IMPBRMaterialManager.hpp"
#include "MMaterialManager.hpp"
#include "MPBRMaterial.hpp"
namespace MEngine::Core::Manager
{
class MPBRMaterialManager final : public MMaterialManager<MPBRMaterial>, public IMPBRMaterialManager
{
  public:
    MPBRMaterialManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator,
                        std::shared_ptr<IMPipelineManager> pipelineManager,
                        std::shared_ptr<IMTextureManager> textureManager)
        : MMaterialManager<MPBRMaterial>(vulkanContext, uuidGenerator, pipelineManager, textureManager)
    {
        CreateDefault();
    }
    ~MPBRMaterialManager() override = default;
    std::shared_ptr<MPBRMaterial> Create(const std::string &name, const std::string &pipelineName,
                                         const MPBRMaterialProperties &properties, const MPBRTextures &textures,
                                         const MPBRMaterialSetting &setting) override;
    std::shared_ptr<MPBRMaterial> CreateLightMaterial() override;
    void Update(std::shared_ptr<MPBRMaterial> material) override;
    void Write(std::shared_ptr<MPBRMaterial> material) override;
    void CreateDefault() override;
    std::shared_ptr<MPBRMaterial> CreateDefaultForwardOpaquePBRMaterial() override;
    virtual void CreateVulkanResources(std::shared_ptr<MPBRMaterial> asset) override;
};
} // namespace MEngine::Core::Manager