#version 330 core

in vec4 vPos;

out vec3 localPos;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    localPos = vPos.xyz;  // this is in local space.
    gl_Position =  projection * modelview * vec4(localPos, 1.0);
}
