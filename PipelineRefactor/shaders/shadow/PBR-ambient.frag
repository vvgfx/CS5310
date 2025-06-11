#version 330


in vec4 outColor;
out vec4 fColor;
uniform vec3 albedo;
uniform float ao;

void main()
{
    outColor = vec4(vec3(0.03f) * albedo * ao, 1.0f);
    // fColor = vec4(1.0, 1.0,1.0, 1.0);
}
