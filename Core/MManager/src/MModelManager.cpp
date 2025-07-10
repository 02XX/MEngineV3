#include "MModelManager.hpp"
#include "MMesh.hpp"
#include "MModel.hpp"
#include "UUID.hpp"
#include <memory>

namespace MEngine::Core::Manager
{

std::shared_ptr<MModel> MModelManager::Create(const std::string &name, const std::vector<UUID> &meshIDs,
                                              const std::vector<UUID> &materialIDs, std::unique_ptr<Node> rootNode,
                                              const MModelSetting &setting)
{
    auto model =
        std::make_shared<MModel>(mUUIDGenerator->Create(), name, meshIDs, materialIDs, std::move(rootNode), setting);
    Update(model);
    mAssets[model->GetID()] = model;
    return model;
}
void MModelManager::Update(std::shared_ptr<MModel> model)
{
    // 填充导航属性
    model->mMeshes =
        model->mMeshIDs |
        std::views::transform([this](const UUID &id) -> std::shared_ptr<MMesh> { return mMeshManager->Get(id); }) |
        std::ranges::to<std::vector<std::shared_ptr<MMesh>>>();
    model->mMaterials = model->mMaterialIDs |
                        std::views::transform([this](const UUID &id) -> std::shared_ptr<MMaterial> {
                            return mMaterialManager->Get(id);
                        }) |
                        std::ranges::to<std::vector<std::shared_ptr<MMaterial>>>();
    mAssets[model->GetID()] = model;
}
std::shared_ptr<MModel> MModelManager::CreateCube()
{

    auto cubeMesh = mMeshManager->GetMesh(DefaultMeshType::Cube);
    auto cubeMaterial = mMaterialManager->CreateDefaultForwardOpaquePBRMaterial();
    std::unique_ptr<Node> rootNode = std::make_unique<Node>();
    rootNode->MeshIndex = 0;
    rootNode->MaterialIndex = 0;
    rootNode->Name = "Cube";               // 根节点名称
    rootNode->Transform = glm::mat4(1.0f); // 单位
    auto meshIDs = std::vector<UUID>{cubeMesh->GetID()};
    auto materialIDs = std::vector<UUID>{cubeMaterial->GetID()};
    auto model = Create("Cube Model", meshIDs, materialIDs, std::move(rootNode), MModelSetting{});
    return model;
}
std::shared_ptr<MModel> MModelManager::CreateSphere()
{
}
std::shared_ptr<MModel> MModelManager::CreatePlane()
{
}
std::shared_ptr<MModel> MModelManager::CreateCylinder()
{
}
void MModelManager::CreateVulkanResources(std::shared_ptr<MModel> model)
{
}
void MModelManager::CreateDefault()
{
    // CreateCube();
}
} // namespace MEngine::Core::Manager