#include "GraphicPipelineManager.hpp"

namespace MEngine::Core
{
void GraphicPipelineManager::Add(std::shared_ptr<GraphicPipeline> asset)
{
    Manager<GraphicPipeline>::Add(asset);
    mNameToID[asset->GetName()] = asset->GetID();
}
std::shared_ptr<GraphicPipeline> GraphicPipelineManager::GetByName(const std::string &name) const
{
    if (mNameToID.contains(name))
    {
        return Get(mNameToID.at(name));
    }
    LogError("Graphic Pipeline with name {} not found", name);
    return nullptr;
}

} // namespace MEngine::Core