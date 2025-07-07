#version 460 core
layout(location = 0) out vec4 OutColor;
layout(location = 2) in vec3 fragNormal; // Location 2
layout(location = 3) in vec2 fragTexCoord; // Location 3

void main()
{
    OutColor = vec4(1.0,0.0,0.0, 1.0); // Set the output color to black
}


