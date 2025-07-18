#version 460 core

layout(location = 0) in vec3 inPosition;  // Location 0
layout(location = 1) in vec3 inNormal;    // Location 1
layout(location = 2) in vec2 inTexCoords; // Location 2
layout(location = 2) out vec2 fragUV;     // 输出纹理坐标
void main()
{
    gl_Position = vec4(inPosition, 1.0);
    fragUV = inTexCoords;
}
