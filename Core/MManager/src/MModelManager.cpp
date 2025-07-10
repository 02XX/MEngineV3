#include "MModelManager.hpp"
#include "MMesh.hpp"
#include "MModel.hpp"
#include "UUID.hpp"
#include <memory>

namespace MEngine::Core::Manager
{

std::shared_ptr<MModel> MModelManager::Create(const MModelSetting &setting, const std::string &name)
{
    return std::make_shared<MModel>(mUUIDGenerator->Create(), name, setting);
}

std::shared_ptr<MModel> MModelManager::CreateCube()
{
    auto model = std::make_shared<MModel>(mUUIDGenerator->Create(), "Cube Model", MModelSetting{});
    auto cubeMesh = mMeshManager->GetMesh(DefaultMeshType::Cube);
    auto cubeMaterial = mMaterialManager->CreateDefaultMaterial();
    model->mMeshes.emplace_back(cubeMesh);
    model->mMaterials.emplace_back(cubeMaterial);
    std::unique_ptr<Node> rootNode = std::make_unique<Node>();
    rootNode->MeshIndex = 0;
    rootNode->MaterialIndex = 0;
    rootNode->Name = "Cube";               // 根节点名称
    rootNode->Transform = glm::mat4(1.0f); // 单位
    model->mRootNode = std::move(rootNode);
    mAssets[model->GetID()] = model;
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