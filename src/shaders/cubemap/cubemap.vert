#version 330

in vec4 vPosition;

uniform mat4 modelview;
uniform mat4 projection;
out vec3 texCoord;

void main()
{
    vec4 pos = projection * modelview * vPosition;
    texCoord = vec3(vPosition);
    gl_Position = pos.xyww;
}
