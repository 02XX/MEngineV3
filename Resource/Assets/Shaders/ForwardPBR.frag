#version 460 core
layout(location = 0) out vec4 OutColor;
layout(location = 2) in vec3 fragNormal; // Location 2
layout(location = 3) in vec2 fragTexCoord; // Location 3
layout(location = 3) uniform light
{
  vec3 position[8];
  vec3 direction[8];
  vec3 color[8];
  float intensity[8];
  float radius[8];
  int type[8];
};
void main()
{
 OutColor = vec4(color[0], 1.0); // Set the output color to black
}


