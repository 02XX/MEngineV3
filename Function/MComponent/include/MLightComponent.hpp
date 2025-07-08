#pragma once
#include "MComponent.hpp"
#include <cstdint>
#include <glm/vec3.hpp>

namespace MEngine::Function::Component
{

enum class LightType : uint32_t
{
    Directional = 0,
    Point = 1,
    Spot = 2,
};
struct MLightComponent final : public MComponent
{
    LightType LightType = LightType::Directional;
    float Intensity = 1.0f;
    glm::vec3 Color = glm::vec3(1.0f);
    // point
    float Radius = 10.0f;
    // spot
    float InnerConeAngle = 0.0f; // in radians
    float OuterConeAngle = 0.0f; // in radians
};

} // namespace MEngine::Function::Component