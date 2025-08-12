#version 460 core

layout(location = 0) in vec4 vPosition;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 worldPos;

void main()
{
    worldPos = model * vPosition;
    gl_Position = projection * view * worldPos;
}
