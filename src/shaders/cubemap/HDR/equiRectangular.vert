#version 330 core
layout (location = 0) in vec4 vPosition;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = vPosition.xyz;
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
