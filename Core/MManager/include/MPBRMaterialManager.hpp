#pragma once
#include "IMPBRMaterialManager.hpp"
#include "MMaterialManager.hpp"
namespace MEngine::Core::Manager
{
class MPBRMaterialManager final : public MMaterialManager<MPBRMaterial, MPBRMaterialSetting>,
                                  public IMPBRMaterialManager
{
  public:
    MPBRMaterialManager(std::shared_ptr<VulkanContext> vulkanContext, std::shared_ptr<IUUIDGenerator> uuidGenerator)
        : MMaterialManager<MPBRMaterial, MPBRMaterialSetting>(vulkanContext, uuidGenerator)
    {
    }
    ~MPBRMaterialManager() override = default;
    std::shared_ptr<MPBRMaterial> Create(const MPBRMaterialSetting &setting) override;
    void Update(std::shared_ptr<MPBRMaterial> material) override
    {
    }
    
};
} // namespace MEngine::Core::Manager