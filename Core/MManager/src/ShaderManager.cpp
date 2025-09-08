#include "ShaderManager.hpp"

namespace MEngine::Core
{
void ShaderManager::Add(std::shared_ptr<Shader> asset)
{
    Manager<Shader>::Add(asset);
    mNameToID[asset->GetName()] = asset->GetID();
}
std::shared_ptr<Shader> ShaderManager::GetByName(const std::string &name) const
{
    if (mNameToID.contains(name))
    {
        return Get(mNameToID.at(name));
    }
    return nullptr;
}
} // namespace MEngine::Core
