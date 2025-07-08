#version 460 core
layout(location = 0) out vec4 OutColor;
layout(location = 2) in vec3 fragNormal; // Location 2
layout(location = 3) in vec2 fragTexCoord; // Location 3

layout(std140, set = 1, binding = 0) uniform MaterialParams {
    vec3 Albedo;
    vec3 Normal;
    float Metallic;
    float Roughness;
    float AO;
    float EmissiveIntensity;
} parameters;

layout(set=1, binding=1) uniform sampler2D albedoMap;
layout(set=1, binding=2) uniform sampler2D normalMap;
layout(set=1, binding=3) uniform sampler2D metallicRoughnessMap;
layout(set=1, binding=4) uniform sampler2D emissiveMap;
void main()
{
    vec3 albedoColor = texture(albedoMap, fragTexCoord).rgb * parameters.Albedo;
    vec3 normalColor = texture(normalMap, fragTexCoord).rgb * parameters.Normal;
    OutColor = vec4(albedoColor, 1.0);
}


