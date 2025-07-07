#pragma once
#include "MComponent.hpp"
#include <entt/entt.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace MEngine::Function::Component
{

struct MTransformComponent : public MComponent
{
    std::string name = "None";

    glm::vec3 localPosition{0.0f, 0.0f, 0.0f};
    glm::quat localRotation = glm::identity<glm::quat>();
    glm::vec3 localScale{1.0f, 1.0f, 1.0f};
    glm::vec3 worldPosition{0.0f, 0.0f, 0.0f};
    glm::quat worldRotation = glm::identity<glm::quat>();
    glm::vec3 worldScale{1.0f, 1.0f, 1.0f};

    glm::mat4 modelMatrix = glm::identity<glm::mat4>();
    bool dirty = true;

    entt::entity parent = entt::null;
    std::vector<entt::entity> children;
};

} // namespace MEngine::Function::Component