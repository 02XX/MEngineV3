#include "MPBRMaterialManager.hpp"
namespace MEngine::Core::Manager
{
std::shared_ptr<MPBRMaterial> MPBRMaterialManager::Create(const MPBRMaterialSetting &setting)
{
    auto pbrMaterial = std::make_shared<MPBRMaterial>(mUUIDGenerator->Create(), setting);
    return pbrMaterial;
}
} // namespace MEngine::Core::Manager