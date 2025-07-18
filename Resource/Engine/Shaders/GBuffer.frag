#version 460 core
struct MaterialParameters
{
    vec3 Albedo;
    vec3 Normal;
    float Metallic;
    float Roughness;
    float AO;
    float EmissiveIntensity;
};
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
struct CameraParameters
{
    vec3 Position;
    vec3 Direction;
    mat4 projectionMatrix;
    mat4 viewMatrix;
};

layout(location = 0) out vec4 OutColor;
layout(location = 1) out vec4 OutNormal;
layout(location = 2) out vec4 OutArm;
layout(location = 3) out vec4 OutPosition;

layout(location = 2) in vec3 fragViewNormal;   // Location 2
layout(location = 3) in vec2 fragTexCoord;     // Location 3
layout(location = 4) in vec3 fragViewPosition; // Location 4

layout(std140, set = 0, binding = 0) uniform CameraUBO
{
    CameraParameters parameters;
}
cameraParams;

layout(std140, set = 1, binding = 0) uniform PBRMaterialParamsUBO
{
    MaterialParameters parameters;
}
materialParameters;

layout(set = 1, binding = 1) uniform sampler2D albedoMap;
layout(set = 1, binding = 2) uniform sampler2D normalMap;
layout(set = 1, binding = 3) uniform sampler2D metallicRoughnessMap;
layout(set = 1, binding = 4) uniform sampler2D emissiveMap;

void main()
{
    vec3 albedo = texture(albedoMap, fragTexCoord).rgb * materialParameters.parameters.Albedo;
    vec3 normal = texture(normalMap, fragTexCoord).rgb * materialParameters.parameters.Normal;
    float ao = texture(metallicRoughnessMap, fragTexCoord).r * materialParameters.parameters.AO;
    float roughness = texture(metallicRoughnessMap, fragTexCoord).g * materialParameters.parameters.Roughness;
    float metallic = texture(metallicRoughnessMap, fragTexCoord).b * materialParameters.parameters.Metallic;
    vec3 emissive = texture(emissiveMap, fragTexCoord).rgb * materialParameters.parameters.EmissiveIntensity;
    vec3 position = fragViewPosition;

    OutColor = vec4(albedo, 1.0);
    OutNormal = vec4(normal, 1.0);
    OutArm = vec4(metallic, roughness, ao, 1.0);
    OutPosition = vec4(position, 1.0);
}
