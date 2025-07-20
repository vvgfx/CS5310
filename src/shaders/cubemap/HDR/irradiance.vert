#version 330 core

layout (location = 0) in vec4 vPos;

out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    WorldPos = vPos.xyz;  
    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}
