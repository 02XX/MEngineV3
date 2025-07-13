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

layout(location = 2) out vec3 fragViewNormal; // Location 2
layout(location = 3) out vec2 fragTexCoords; // Location 3
layout(location = 4) out vec3 fragViewPosition; // Location 4
layout(std140, set = 0, binding = 0) uniform CameraUBO
{
    CameraParameters parameters;
} cameraParams;
layout(push_constant) uniform PushConstant
{
    mat4 modelMatrix;
} pushConstants;
void main()
{
   
    fragTexCoords = inTexCoords;
    fragViewNormal =  (normalize((cameraParams.parameters.viewMatrix * pushConstants.modelMatrix * vec4(inNormal, 0.0)).xyz)); // Transform normal to view space
    vec4 viewPosition = (cameraParams.parameters.viewMatrix * pushConstants.modelMatrix * vec4(inPosition, 1.0)); // Transform position to view space
    fragViewPosition = viewPosition.xyz; 
    gl_Position = cameraParams.parameters.projectionMatrix * viewPosition;
}


