#version 330

in vec4 vPosition;
uniform mat4 modelview;
uniform mat4 projection;

void main()
{
    gl_Position = projection * modelview * vPosition; // only need this to render the object into the depth buffer.
}
