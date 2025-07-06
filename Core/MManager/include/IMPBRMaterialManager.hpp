#pragma once
#include "IMMaterialManager.hpp"
#include "MPBRMaterial.hpp"
namespace MEngine::Core::Manager
{
class IMPBRMaterialManager : public IMMaterialManager<MPBRMaterial, MPBRMaterialSetting>
{
  public:
    ~IMPBRMaterialManager() override = default;
};
} // namespace MEngine::Core::Manager