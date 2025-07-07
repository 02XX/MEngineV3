#include "MCameraSystem.hpp"
#include "MCameraComponent.hpp"
#include "MTransformComponent.hpp"
#include <entt/entt.hpp>
#include <glm/glm.hpp>
using namespace MEngine::Function::Component;
namespace MEngine::Function::System
{
void MCameraSystem::Init()
{
}
void MCameraSystem::Update(float deltaTime)
{
    auto view = mRegistry->view<MCameraComponent, MTransformComponent>();
    for (auto entity : view)
    {
        auto &cameraComponent = view.get<MCameraComponent>(entity);
        auto &transformComponent = view.get<MTransformComponent>(entity);
        auto &camera = view.get<MCameraComponent>(entity);
        glm::mat4 viewMatrix = glm::lookAtRH(transformComponent.worldPosition, camera.target, camera.up);
        auto rotationMatrix = glm::mat4_cast(transformComponent.worldRotation);
        camera.viewMatrix = rotationMatrix * viewMatrix;
        camera.projectionMatrix = glm::perspective(glm::radians(camera.fovY / camera.zoom), camera.aspectRatio,
                                                   camera.nearPlane, camera.farPlane);
    }
}
void MCameraSystem::Shutdown()
{
}
} // namespace MEngine::Function::System