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
    std::string Name{};                            // Name of the node
    glm::mat4 Transform{1.0f};                     // Transformation matrix of the node
    int MeshIndex = -1;                       // Index of the mesh in the model
    int MaterialIndex = -1;                   // Index of the material in the model
    Node *Parent = nullptr;                        // Pointer to the parent node, nullptr if root
    std::vector<std::unique_ptr<Node>> Children{}; // Indices of child nodes
};
class MModel : public MAsset
{
    friend class nlohmann::adl_serializer<MModel>;
    friend class Manager::MModelManager;

  private:
    std::vector<std::shared_ptr<MMesh>> mMeshes{};
    std::vector<std::shared_ptr<MMaterial>> mMaterials{};
    std::unique_ptr<Node> mRootNode{};
    MModelSetting mSetting{};

  public:
    MModel(const UUID &id, const std::string &name, const MModelSetting &setting) : MAsset(id, name), mSetting(setting)
    {
        mType = MAssetType::Model;
        mState = MAssetState::Unloaded;
    }
    inline const Node *GetRootNode() const
    {
        return mRootNode.get();
    }
    inline void SetNode(std::unique_ptr<Node> root)
    {
        mRootNode = std::move(root);
    }
    inline const std::vector<std::shared_ptr<MMesh>> &GetMeshes() const
    {
        return mMeshes;
    }
    inline void SetMeshes(const std::vector<std::shared_ptr<MMesh>> &meshes)
    {
        mMeshes = meshes;
    }

    inline const std::vector<std::shared_ptr<MMaterial>> &GetMaterials() const
    {
        return mMaterials;
    }
    inline void SetMaterials(const std::vector<std::shared_ptr<MMaterial>> &materials)
    {
        mMaterials = materials;
    }
};
} // namespace MEngine::Core::Asset
