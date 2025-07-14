#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 3) in vec4 vTexCoord;

out vec2 fTexCoords;

void main()
{
    fTexCoords = vTexCoord.st;
    gl_Position = vPosition;
}
