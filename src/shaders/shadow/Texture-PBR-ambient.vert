#version 330

layout(location = 0) in vec4 vPosition;
layout(location = 3) in vec4 vTexCoord;
uniform mat4 modelview;
uniform mat4 projection;
uniform mat4 texturematrix;

out vec4 fTexCoord;

void main()
{
    gl_Position = projection * modelview * vPosition;
    fTexCoord = texturematrix * vec4(1*vTexCoord.s,1*vTexCoord.t,0,1);
}
