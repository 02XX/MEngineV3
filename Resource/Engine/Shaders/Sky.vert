#version 460 core

struct CameraParameters
{
    vec3 Position;
    vec3 Direction;
    mat4 projectionMatrix;
    mat4 viewMatrix;
};
layout(location = 0) in vec3 inPosition;  // Location 0
layout(location = 1) in vec3 inNormal;    // Location 1
layout(location = 2) in vec2 inTexCoords; // Location 2

layout(location = 2) out vec3 fragViewNormal;   // Location 2
layout(location = 3) out vec2 fragTexCoords;    // Location 3
layout(location = 4) out vec3 fragViewPosition; // Location 4
layout(location = 5) out vec3 fragDirection;    // Location 5
layout(std140, set = 0, binding = 0) uniform CameraUBO
{
    CameraParameters parameters;
}
cameraParams;
layout(push_constant) uniform PushConstant
{
    mat4 modelMatrix;
}
pushConstants;

void main()
{
    mat3 rotationMatrix = mat3(cameraParams.parameters.viewMatrix);
    fragDirection = rotationMatrix * inPosition;                                    // Transform direction to view space
    gl_Position = cameraParams.parameters.projectionMatrix * vec4(inPosition, 1.0); // Transform position to clip space
    gl_Position.z = gl_Position.w; // Ensure depth is set correctly
}