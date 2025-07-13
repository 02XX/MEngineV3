#version 460 core
const float PI = 3.14159265359f;
vec2 DirectionToUV(vec3 dir)
{
    dir = normalize(dir);

    float u = atan(dir.z, dir.x);
    u = (u + PI) / (2.0 * PI);

    float v = asin(dir.y);
    v = (v + PI / 2.0) / PI;

    return vec2(u, v);
}
layout(location = 0) out vec4 OutColor;

layout(location = 5) in vec3 vDirection;

layout(set = 0, binding = 2) uniform sampler2D environmentMap;

void main()
{
    OutColor = pow(texture(environmentMap, DirectionToUV(vDirection)), vec4(2.2));
}
