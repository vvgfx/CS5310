#version 460 core

// Expand non-empty voxels into 6-face cubes; empty voxels emit nothing.

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 gridMin;
uniform vec4 gridMax;
uniform float voxelResolution;
uniform float alphaThreshold;

in VoxelData {
    vec4 color;
    vec3 worldCenter;
} vs_in[];

out VoxelFrag {
    vec3 color;
    vec3 normal;
} gs_out;

void emitFace(mat4 vp, vec3 center, vec3 h, vec3 n,
              vec3 c0, vec3 c1, vec3 c2, vec3 c3, vec3 color)
{
    gs_out.color  = color;
    gs_out.normal = n;
    gl_Position = vp * vec4(center + c0 * h, 1.0); EmitVertex();
    gl_Position = vp * vec4(center + c1 * h, 1.0); EmitVertex();
    gl_Position = vp * vec4(center + c2 * h, 1.0); EmitVertex();
    gl_Position = vp * vec4(center + c3 * h, 1.0); EmitVertex();
    EndPrimitive();
}

void main()
{
    vec4 voxel = vs_in[0].color;
    if (voxel.a <= alphaThreshold)
        return;

    mat4 vp = projection * view;
    vec3 center = vs_in[0].worldCenter;
    vec3 h = 0.5 * (gridMax.xyz - gridMin.xyz) / voxelResolution;
    vec3 rgb = voxel.rgb;

    // Corners in ±1 space; h scales to world-space half-extents. Culling is off.
    emitFace(vp, center, h, vec3( 1, 0, 0),
             vec3( 1,-1,-1), vec3( 1, 1,-1), vec3( 1,-1, 1), vec3( 1, 1, 1), rgb);
    emitFace(vp, center, h, vec3(-1, 0, 0),
             vec3(-1,-1,-1), vec3(-1,-1, 1), vec3(-1, 1,-1), vec3(-1, 1, 1), rgb);
    emitFace(vp, center, h, vec3( 0, 1, 0),
             vec3(-1, 1,-1), vec3(-1, 1, 1), vec3( 1, 1,-1), vec3( 1, 1, 1), rgb);
    emitFace(vp, center, h, vec3( 0,-1, 0),
             vec3(-1,-1,-1), vec3( 1,-1,-1), vec3(-1,-1, 1), vec3( 1,-1, 1), rgb);
    emitFace(vp, center, h, vec3( 0, 0, 1),
             vec3(-1,-1, 1), vec3( 1,-1, 1), vec3(-1, 1, 1), vec3( 1, 1, 1), rgb);
    emitFace(vp, center, h, vec3( 0, 0,-1),
             vec3(-1,-1,-1), vec3(-1, 1,-1), vec3( 1,-1,-1), vec3( 1, 1,-1), rgb);
}
