#version 460 core

// Directional face shading so cube faces read as distinct surfaces.

in VoxelFrag {
    vec3 color;
    vec3 normal;
} fs_in;

out vec4 fColor;

void main()
{
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
    float shade = 0.55 + 0.45 * max(0.0, dot(fs_in.normal, lightDir));
    fColor = vec4(fs_in.color * shade, 1.0);
}
