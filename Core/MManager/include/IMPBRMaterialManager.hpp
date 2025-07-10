#pragma once
#include "IMMaterialManager.hpp"
#include "MPBRMaterial.hpp"
#include <memory>
namespace MEngine::Core::Manager
{
class IMPBRMaterialManager : public virtual IMMaterialManager<MPBRMaterial>
{
  public:
    ~IMPBRMaterialManager() override = default;
    virtual std::shared_ptr<MPBRMaterial> CreateDefaultForwardOpaquePBRMaterial() = 0;
    virtual std::shared_ptr<MPBRMaterial> Create(const std::string &name, const std::string &pipelineName,
                                                 const MPBRMaterialProperties &properties, const MPBRTextures &textures,
                                                 const MPBRMaterialSetting &setting) = 0;
};
} // namespace MEngine::Core::Manager