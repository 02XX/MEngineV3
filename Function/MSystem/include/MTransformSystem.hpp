#pragma once
#include "MSystem.hpp"
#include "MTransformComponent.hpp"
#include <entt/entt.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/glm.hpp>
#include <memory>
using namespace MEngine::Function::Component;
namespace MEngine::Function::System
{
class MTransformSystem final : public MSystem
{
  private:
  public:
    MTransformSystem(std::shared_ptr<entt::registry> registry, std::shared_ptr<ResourceManager> resourceManager)
        : MSystem(registry, resourceManager)
    {
    }
    void Init() override;
    void Update(float deltaTime) override;
    void Shutdown() override;

    static void Translate(MTransformComponent &transform, const glm::vec3 &delta);
    static void Rotate(MTransformComponent &transform, float angle, const glm::vec3 &axis);
    static void Scale(MTransformComponent &transform, const glm::vec3 &scale);
    static void SetWorldPosition(MTransformComponent &transform, const glm::vec3 &position);
    static void SetWorldRotation(MTransformComponent &transform, const glm::quat &rotation);
    static void SetWorldScale(MTransformComponent &transform, const glm::vec3 &scale);
    static void SetModelMatrix(MTransformComponent &transform, const glm::mat4 &modelMatrix,
                               const glm::mat4 &parentMatrix = glm::mat4(1.0f));

  private:
    void CalculateMatrix(entt::entity entity);
};
} // namespace MEngine::Function::System