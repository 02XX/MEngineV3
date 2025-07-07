#pragma once
#include "MComponent.hpp"
#include <glm/vec3.hpp>

namespace MEngine::Function::Component
{

enum class LightType
{
    Directional,
    Point,
    Spot,
};
struct MLightComponent final : public MComponent
{
    LightType LightType = LightType::Directional;
    float Intensity = 1.0f;
    glm::vec3 Color = glm::vec3(1.0f);
    // point
    float Radius = 10.0f;
};

} // namespace MEngine::Function::Component