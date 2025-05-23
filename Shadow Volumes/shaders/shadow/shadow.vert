#version 330

in vec4 vPosition;
out vec3 gPosition;

uniform mat4 projection;
uniform mat4 modelview;

void main()
{
    gPosition = vec3(modelview * vPosition); // remember-view coordinates already
    // gPosition = vec3(vPosition);
    // gl_Position = projection * modelview * vPosition;
}
