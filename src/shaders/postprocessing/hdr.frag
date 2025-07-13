#version 330 core

in vec2 fTexCoords;
uniform sampler2D hdrColorBuffer;

out vec4 fragColor;

void main()
{
    vec3 hdrColor = texture(hdrColorBuffer, fTexCoords).rgb;
    fragColor = vec4(hdrColor, 1.0);
}
