#version 330 core


in vec4 vPosition;
in vec4 vNormal;
in vec4 vTexCoord;

out vec2 fTexCoords;

void main()
{
    fTexCoords = vTexCoord.st;
    gl_Position = vPosition;
}
