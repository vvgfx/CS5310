#version 330

in vec4 fTexCoord;
uniform sampler2D albedoMap;
uniform sampler2D aoMap;

out vec4 fColor;

void main()
{
    vec3 albedo = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2)); // convert sRGB to linear space!
    float ao = texture(aoMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    fColor = vec4(vec3(0.03f) * albedo * ao, 1.0f);
}
