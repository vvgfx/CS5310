#version 330

out vec4 fColor;
uniform vec3 albedo;
uniform float ao;

void main()
{
    fColor = vec4(vec3(0.03f) * albedo * ao, 1.0f);
}
