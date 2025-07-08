#version 460 core
// input vertex data
layout(location = 0) in vec3 inPosition;  // Location 0
layout(location = 1) in vec3 inNormal;    // Location 1
layout(location = 2) in vec2 inTexCoords; // Location 2

layout(location = 2) out vec3 fragNormal; // Location 2
layout(location = 3) out vec2 fragTexCoords; // Location 3

layout(std140,set = 0, binding = 0) uniform CameraParameters
{
    mat4 projectionMatrix;
    mat4 viewMatrix;
} cameraParams;
layout(push_constant) uniform PushConstant
{
    mat4 modelMatrix;
} pushConstants;
void main()
{
    gl_Position = cameraParams.projectionMatrix * cameraParams.viewMatrix * pushConstants.modelMatrix * vec4(inPosition, 1.0);
}


