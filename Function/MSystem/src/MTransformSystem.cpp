#include "MTransformSystem.hpp"
#include "Math.hpp"
using namespace MEngine::Function::Component;
namespace MEngine::Function::System
{
void MTransformSystem::Init()
{
}
void MTransformSystem::Update(float deltaTime)
{
    auto view = mRegistry->view<MTransformComponent>();
    for (auto entity : view)
    {
        auto &transformComponent = view.get<MTransformComponent>(entity);
        CalculateMatrix(entity);
    }
}
void MTransformSystem::Shutdown()
{
}
void MTransformSystem::CalculateMatrix(entt::entity entity)
{
    // 获取实体的TransformComponent
    auto &transformComponent = mRegistry->get<MTransformComponent>(entity);
    // local
    glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), transformComponent.localPosition) *
                            glm::mat4_cast(transformComponent.localRotation) *
                            glm::scale(glm::mat4(1.0f), transformComponent.localScale);
    if (transformComponent.parent != entt::null)
    {
        auto &parentTransform = mRegistry->get<MTransformComponent>(transformComponent.parent);
        transformComponent.modelMatrix = parentTransform.modelMatrix * localMatrix;
    }
    else
    {
        transformComponent.modelMatrix = localMatrix;
    }

    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(transformComponent.modelMatrix, transformComponent.worldScale, transformComponent.worldRotation,
                   transformComponent.worldPosition, skew, perspective);

    // 递归更新所有子节点
    for (auto child : transformComponent.children)
    {
        CalculateMatrix(child);
    }
}
void MTransformSystem::Translate(MTransformComponent &transform, const glm::vec3 &delta)
{
    transform.localPosition += delta;
    transform.dirty = true;
}
void MTransformSystem::Rotate(MTransformComponent &transform, float angle, const glm::vec3 &axis)
{
}
void MTransformSystem::Scale(MTransformComponent &transform, const glm::vec3 &localScale)
{
}
void MTransformSystem::SetWorldPosition(MTransformComponent &transform, const glm::vec3 &position)
{
}
void MTransformSystem::SetWorldRotation(MTransformComponent &transform, const glm::quat &rotation)
{
}
void MTransformSystem::SetWorldScale(MTransformComponent &transform, const glm::vec3 &scale)
{
}
void MTransformSystem::SetModelMatrix(MTransformComponent &transform, const glm::mat4 &modelMatrix,
                                      const glm::mat4 &parentMatrix)
{
    transform.modelMatrix = modelMatrix;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(modelMatrix, transform.worldScale, transform.worldRotation, transform.worldPosition, skew,
                   perspective);
    auto localMatrix = glm::inverse(parentMatrix) * modelMatrix;
    glm::decompose(localMatrix, transform.localScale, transform.localRotation, transform.localPosition, skew,
                   perspective);
}
} // namespace MEngine::Function::System