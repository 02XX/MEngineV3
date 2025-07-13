#version 460 core
#define MAX_LIGHT_COUNT 6
const float PI = 3.14159265359f;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NOH = clamp(dot(N, H), 0, 1.f);
    float NOH2 = NOH * NOH;
    float nom = a2;
    float demom = (NOH2 * (a2 - 1.0f) + 1.0f);
    demom = PI * demom * demom;
    return nom / demom;
}
float GeometrySchlickGGX(float NoV, float roughness)
{
    float r = roughness + 1.0f;
    float k = r * r / 8.0f;
    float nom = NoV;
    float denom = NoV * (1.0f - k) + k;
    return nom / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NoV = clamp(dot(N, V), 0, 1.f);
    float NoL = clamp(dot(N, L), 0, 1.f);
    float ggx2 = GeometrySchlickGGX(NoV, roughness);
    float ggx1 = GeometrySchlickGGX(NoL, roughness);
    return ggx1 * ggx2;
}
vec3 FresnelSchlick(float HoV, vec3 F0)
{
    // 钳制是否正确
    return F0 + (1.0 - F0) * pow(clamp(1.0 - HoV, 0.0, 1.0), 5.0);
}
float RadicalInverse_VdC(uint bits)
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}
vec3 uniformSampleHemisphere(vec2 Xi, vec3 N)
{
    float theta = acos(1.0 - Xi.x);
    float phi = 2.0f * PI * Xi.y;
    float x = sin(theta) * cos(phi);
    float y = sin(phi) * sin(theta);
    float z = cos(theta);
    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 0.1, 0.0) : vec3(0.0, 0.0, 1.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN = mat3(tangent, bitangent, N);
    vec3 dir = TBN * vec3(x, y, z);
    return dir;
}
vec3 ImportanceSampleGGX(vec3 N, float roughness, vec2 Xi)
{
    float a = roughness * roughness;
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    float x = sinTheta * cos(phi);
    float y = sin(phi) * sinTheta;
    float z = cosTheta;
    vec3 up = abs(N.y) < 0.999 ? vec3(0.0, 0.1, 0.0) : vec3(0.0, 0.0, 1.0);
    vec3 tangent = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN = mat3(tangent, bitangent, N);
    vec3 dir = TBN * vec3(x, y, z);
    return dir;
}
vec2 DirectionToUV(vec3 dir)
{
    dir = normalize(dir);

    float u = atan(dir.z, dir.x);
    u = (u + PI) / (2.0 * PI);

    float v = asin(dir.y);
    v = (v + PI / 2.0) / PI;

    return vec2(u, v);
}
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
layout(location = 2) in vec3 fragViewNormal;   // Location 2
layout(location = 3) in vec2 fragTexCoord;     // Location 3
layout(location = 4) in vec3 fragViewPosition; // Location 4

layout(std140, set = 0, binding = 0) uniform CameraUBO
{
    CameraParameters parameters;
}
cameraParams;
layout(std140, set = 0, binding = 1) uniform LightUBO
{
    LightParameters parameters[MAX_LIGHT_COUNT];
}
lights;

layout(set = 0, binding = 2) uniform sampler2D environmentMap;
layout(set = 0, binding = 3) uniform sampler2D irradianceMap;
layout(set = 0, binding = 4) uniform sampler2D brdfLUT;
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
    vec3 albedoColor = texture(albedoMap, fragTexCoord).rgb * materialParameters.parameters.Albedo;
    vec3 normalColor = texture(normalMap, fragTexCoord).rgb * materialParameters.parameters.Normal;
    vec3 arm = texture(metallicRoughnessMap, fragTexCoord).rgb;
    float ao = arm.r * materialParameters.parameters.AO;
    float roughness = arm.g * materialParameters.parameters.Roughness;
    float metallic = arm.b * materialParameters.parameters.Metallic;
    vec4 finalColor = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 VIEW = -fragViewPosition; // 相机空间
    vec3 V = normalize(VIEW);
    vec3 N = normalize(normalColor * 2.0 - 1.0);
    vec3 F0 = mix(vec3(0.04), albedoColor, metallic);
    float NoV = clamp(dot(N, V), 0, 1.0f);
    int lightCount = 0;
    for (int i = 0; i < MAX_LIGHT_COUNT; i++)
    {
        if (lights.parameters[i].Enable == 0)
            continue;
        lightCount++;
        vec3 LIGHT_COLOR = lights.parameters[i].Color * lights.parameters[i].Intensity;
        if (lights.parameters[i].LightType == 0) // 平行光
        {
            vec4 lightDirection4 = cameraParams.parameters.viewMatrix *
                                   vec4(lights.parameters[i].Direction, 0.0); // Convert to 4-component vector
            vec3 LIGHT = -normalize(lightDirection4.xyz); // Extract 3-component vector and normalize
            vec3 L = normalize(LIGHT);
            vec3 H = normalize(L + V);
            float VoH = clamp(dot(V, H), 0, 1.0f);
            float NoL = clamp(dot(N, L), 0, 1.0f);
            if (NoV > 0.0f)
            {
                vec3 F = FresnelSchlick(VoH, F0);
                float D = DistributionGGX(N, H, roughness);
                float G = GeometrySmith(N, V, L, roughness);
                vec3 numerator = F * D * G;
                float denominator = 4.0 * NoV * NoL + 1e-5;
                vec3 specular = numerator / denominator;
                vec3 kS = F;
                vec3 kD = vec3(1.0) - kS;
                kD *= 1.0 - metallic;
                finalColor += vec4((kD * albedoColor / PI + specular) * (LIGHT_COLOR)*NoL, 1.0);
            }
        }
        else if (lights.parameters[i].LightType == 1) // 点光源
        {
        }
        else if (lights.parameters[i].LightType == 2) // 聚光灯
        {
        }
    }
    finalColor = finalColor / float(lightCount);

    float maxMipLevel = log2(textureSize(environmentMap, 0).x);
    float lod = roughness * maxMipLevel;
    vec2 uv = DirectionToUV(normalize(fragViewNormal));
    vec3 environmentRadiance = pow(textureLod(environmentMap, uv, lod).rgb, vec3(2.2));
    vec3 irradiance = pow(texture(irradianceMap, uv).rgb, vec3(2.2));
    vec3 brdf = texture(brdfLUT, vec2(NoV, roughness)).rgb;
    vec3 kS = F0;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;
    vec3 diffuse = irradiance * albedoColor * kD;
    vec3 specular = environmentRadiance * (F0 * brdf.x + brdf.y);
    vec3 ambient = kD * diffuse + kS * specular;
    finalColor += vec4(ambient, 1.0) * ao;
    OutColor = finalColor;
}
