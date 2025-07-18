#version 330 core

layout (location = 0) in vec3 vPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    localPos = vPos;  // this is in local space.
    gl_Position =  projection * modelview * vec4(localPos, 1.0);
}
