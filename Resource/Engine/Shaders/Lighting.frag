#version 460 core
#define MAX_LIGHT_COUNT 6
struct LightParameters
{
    int LightType; // 0-平行光，1-点光，2-聚光灯

    // point
    float Intensity;
    float Radius;
    // spot
    float InnerConeAngle; // in radians
    float OuterConeAngle; // in radians
    int Enable;           // 是否启用光源，1表示启用，0表示禁用

    vec3 Color;
    vec3 Position;
    vec3 Direction;
};

layout(location = 0) out vec4 OutColor;
layout(location = 2) in vec2 fragUV;

layout(input_attachment_index = 0, set = 1, binding = 0) uniform subpassInput albedoInput;
layout(input_attachment_index = 1, set = 1, binding = 1) uniform subpassInput normalInput;
layout(input_attachment_index = 2, set = 1, binding = 2) uniform subpassInput armInput;
layout(input_attachment_index = 3, set = 1, binding = 3) uniform subpassInput positionInput;

layout(std140, set = 0, binding = 1) uniform LightUBO
{
    LightParameters parameters[MAX_LIGHT_COUNT];
}
lights;
void main()
{
    vec4 albedo = subpassLoad(albedoInput);
    vec3 normal = normalize(subpassLoad(normalInput).xyz);
    vec4 arm = subpassLoad(armInput);
    vec4 position = subpassLoad(positionInput);
    float ao = arm.r;
    float roughness = arm.g;
    float metallic = arm.b;

    OutColor = vec4(albedo.rgb, 1.0);
}