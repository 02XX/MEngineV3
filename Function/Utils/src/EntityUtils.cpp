#include "EntityUtils.hpp"
#include "MMaterialComponent.hpp"
#include "MMeshComponent.hpp"
#include "MTransformComponent.hpp"
#include <entt/entt.hpp>
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
        auto &meshComponent = registry->emplace<MMeshComponent>(entity, MMeshComponent{});
        meshComponent.meshID = meshes[currentNode->MeshIndex]->GetID();
        meshComponent.mesh = meshes[currentNode->MeshIndex];
  
        auto &materialComponent = registry->emplace<MMaterialComponent>(entity, MMaterialComponent{});
        materialComponent.materialID = materials[currentNode->MaterialIndex]->GetID();
        materialComponent.material = materials[currentNode->MaterialIndex];
        for (auto child : currentNode->Children)
        {
            auto childEntity = createNode(child);
            auto &childTransformComponent = registry->get<MTransformComponent>(childEntity);
            childTransformComponent.parent = entity;
            transformComponent.children.push_back(childEntity);
        }
        return entity;
    };
    return createNode(rootNode);
}
} // namespace MEngine::Function::Utils
