#version 460 core
// input vertex data
layout(location = 0) in vec3 inPosition;  // Location 0
layout(location = 1) in vec3 inNormal;    // Location 1
layout(location = 2) in vec2 inTexCoords; // Location 2

layout(location = 0) uniform mat4 viewMatrix; 
layout(location = 1) uniform mat4 projectionMatrix;       
layout(location = 2) uniform mat4 modelMatrix;    

layout(location = 2) out vec3 fragNormal; // Location 2
layout(location = 3) out vec2 fragTexCoords; // Location 3

void main()
{
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(inPosition, 1.0);
    fragNormal = mat3(modelMatrix) * inNormal; // 变换法线
    fragTexCoords = inTexCoords; // 传递纹理坐标
}


