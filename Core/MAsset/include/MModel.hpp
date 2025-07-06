#pragma once
#include "MAsset.hpp"
#include "MMaterial.hpp"
#include "MMesh.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan_handles.hpp>

namespace MEngine::Core::Asset
{
class MModelSetting : public MAssetSetting
{
  public:
    ~MModelSetting() override = default;
};
struct Node
{
    std::vector<uint32_t> Children{}; // Indices of child nodes
    uint32_t MeshIndex = -1;          // Index of the mesh in the model
};
class MModel : public MAsset
{
    friend class nlohmann::adl_serializer<MModel>;
    friend class Manager::MModelManager;

  private:
    std::vector<std::shared_ptr<MMesh>> mMeshes{};
    std::vector<std::shared_ptr<MMaterial>> mMaterials{};
    Node mRootNode{};
    MModelSetting mSetting{};

  public:
    MModel(const UUID &id, const MModelSetting &setting) : MAsset(id), mSetting(setting)
    {
        mType = MAssetType::Model;
        mState = MAssetState::Unloaded;
    }
    inline const Node &GetRootNode() const
    {
        return mRootNode;
    }
    inline void SetRootNode(const Node &rootNode)
    {
        mRootNode = rootNode;
    }
};
} // namespace MEngine::Core::Asset
