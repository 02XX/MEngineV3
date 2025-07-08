#include "gtest/gtest.h"
#include <glm/glm.hpp>
#include <gtest/gtest.h>

TEST(AlignTest, Align16)
{
    struct LightParameters
    {
        glm::vec3 Color = glm::vec3(1.0f);
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
        float Intensity = 1.0f;
        float Radius = 10.0f;
        float InnerConeAngle = 0.0f;
        float OuterConeAngle = 0.0f;
        int enable = 0;
    };
    GTEST_LOG_(INFO) << "Size of LightParameters: " << sizeof(LightParameters)
                     << " glm::vec3 align: " << alignof(LightParameters::Color)
                     << " struct align: " << alignof(LightParameters);
    struct alignas(16) LightParameters2
    {
        float Intensity = 1.0f;
        float Radius = 10.0f;
        float InnerConeAngle = 0.0f;
        float OuterConeAngle = 0.0f;
        int enable = 0;
        glm::vec3 Color = glm::vec3(1.0f);
        glm::vec3 Position = glm::vec3(0.0f);
        glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);
    };
    GTEST_LOG_(INFO) << "Size of LightParameters2: " << sizeof(LightParameters2)
                     << " glm::vec3 align: " << alignof(LightParameters2::Color)
                     << " struct align: " << alignof(LightParameters2);
}