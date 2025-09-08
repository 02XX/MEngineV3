#include "PipelineLayoutManager.hpp"
#include "Manager.hpp"

namespace MEngine::Core
{
void PipelineLayoutManager::Add(std::shared_ptr<PipelineLayout> asset)
{
    Manager<PipelineLayout>::Add(asset);
    mTypeToID[asset->GetPipelineLayoutType()] = asset->GetID();
}

std::shared_ptr<PipelineLayout> PipelineLayoutManager::GetByType(PipelineLayoutType type) const
{
    if (mTypeToID.contains(type))
    {
        return Get(mTypeToID.at(type));
    }
    return nullptr;
}

} // namespace MEngine::Core
