#include "EntityUtils.hpp"
#include "MMaterialComponent.hpp"
#include "MMeshComponent.hpp"
#include "MModel.hpp"
#include "MTransformComponent.hpp"
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <vector>
using namespace MEngine::Function::Component;
namespace MEngine::Function::Utils
{
entt::entity EntityUtils::CreateEntity(std::shared_ptr<entt::registry> registry, std::shared_ptr<MModel> model)
{
    auto rootNode = model->GetRootNode();
    auto meshes = model->GetMeshes();
    auto materials = model->GetMaterials();
    std::function<entt::entity(const Node *)> createNode = [&](const Node *currentNode) -> entt::entity {
        entt::entity entity = registry->create();
        auto &transformComponent = registry->emplace<MTransformComponent>(entity, MTransformComponent{});
        transformComponent.name = currentNode->Name;
        // TODO : Handle transform properly
        if (currentNode->MeshIndex != -1 && currentNode->MaterialIndex != -1)
        {
            auto &meshComponent = registry->emplace<MMeshComponent>(entity, MMeshComponent{});
            meshComponent.meshID = meshes[currentNode->MeshIndex]->GetID();
            meshComponent.mesh = meshes[currentNode->MeshIndex];

            auto &materialComponent = registry->emplace<MMaterialComponent>(entity, MMaterialComponent{});
            materialComponent.materialID = materials[currentNode->MaterialIndex]->GetID();
            materialComponent.material = materials[currentNode->MaterialIndex];
        }
        for (auto &child : currentNode->Children)
        {
            auto childEntity = createNode(child.get());
            auto &childTransformComponent = registry->get<MTransformComponent>(childEntity);
            childTransformComponent.parent = entity;
            transformComponent.children.push_back(childEntity);
        }
        return entity;
    };
    return createNode(rootNode);
}
std::shared_ptr<MModel> EntityUtils::GetModelFromEntity(std::shared_ptr<Resource::ResourceManager> resourceManager,
                                                        std::shared_ptr<entt::registry> registry, entt::entity entity)
{
    auto modelManager = resourceManager->GetManager<MModel, IMModelManager>();
    std::function<std::unique_ptr<Node>(entt::entity)> processEntity;
    std::vector<std::shared_ptr<MMesh>> modelMeshes{};
    std::vector<std::shared_ptr<MMaterial>> modelMaterials{};
    std::vector<UUID> meshIDs{};
    std::vector<UUID> materialIDs{};
    processEntity = [&](entt::entity entity) -> std::unique_ptr<Node> {
        if (registry->any_of<MTransformComponent>(entity))
        {
            auto currentNode = std::make_unique<Node>();
            auto &transform = registry->get<MTransformComponent>(entity);
            currentNode->Name = transform.name;
            currentNode->Transform = transform.modelMatrix;
            if (registry->any_of<MMeshComponent>(entity))
            {
                auto &meshComponent = registry->get<MMeshComponent>(entity);
                currentNode->MeshIndex = static_cast<int>(modelMeshes.size());
                modelMeshes.push_back(meshComponent.mesh);
                meshIDs.push_back(meshComponent.mesh->GetID());
            }
            if (registry->any_of<MMaterialComponent>(entity))
            {
                auto &materialComponent = registry->get<MMaterialComponent>(entity);
                currentNode->MaterialIndex = static_cast<int>(modelMaterials.size());
                modelMaterials.push_back(materialComponent.material);
                materialIDs.push_back(materialComponent.material->GetID());
            }
            for (auto child : transform.children)
            {
                if (registry->valid(child))
                {
                    currentNode->Children.push_back(processEntity(child));
                }
            }
            return currentNode;
        }
        return nullptr;
    };
    auto rootNode = processEntity(entity);
    if (rootNode)
    {
        auto name = rootNode->Name.empty() ? "Unnamed Model" : rootNode->Name;
        auto model = modelManager->Create(name, meshIDs, materialIDs, std::move(rootNode), MModelSetting{});
        modelManager->Update(model);
        return model;
    }
    return nullptr;
}
} // namespace MEngine::Function::Utils
