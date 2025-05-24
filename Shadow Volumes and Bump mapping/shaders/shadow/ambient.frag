#version 330


in vec4 outColor;
out vec4 fColor;

void main()
{
    fColor = outColor * 0.2f;
    // fColor = vec4(1.0, 1.0,1.0, 1.0);
}
