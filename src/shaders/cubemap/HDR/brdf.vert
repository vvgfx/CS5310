#version 330 core
layout (location = 0) in vec4 vPos;
layout (location = 3) in vec4 vTexCoord;

out vec2 TexCoords;

void main()
{
    TexCoords = vTexCoord.st;
	gl_Position = vPos;
}
