#version 330 core

in vec2 fTexCoords;
uniform sampler2D hdrColorBuffer;
uniform float exposure;

out vec4 fragColor;

void main()
{
    const float gamma = 2.2; // standard for sRGB space
    vec3 hdrColor = texture(hdrColorBuffer, fTexCoords).rgb;

    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
    result = pow(result , vec3(1.0 / gamma));
    fragColor = vec4(result, 1.0);
}
