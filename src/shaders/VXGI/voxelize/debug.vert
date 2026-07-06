#version 460 core

// One point per voxel; VS looks up voxel data via gl_VertexID, GS emits a cube.

uniform sampler3D voxelTexture;
uniform vec4 gridMin;
uniform vec4 gridMax;
uniform float voxelResolution;

out VoxelData {
    vec4 color;
    vec3 worldCenter;
} vs_out;

void main()
{
    int res  = int(voxelResolution);
    int vid  = gl_VertexID;
    int x    =  vid                % res;
    int y    = (vid / res)         % res;
    int z    =  vid / (res * res);

    vs_out.color = texelFetch(voxelTexture, ivec3(x, y, z), 0);

    vec3 voxelSize = (gridMax.xyz - gridMin.xyz) / voxelResolution;
    vs_out.worldCenter = gridMin.xyz + (vec3(x, y, z) + 0.5) * voxelSize;

    // Geom shader overwrites gl_Position per cube corner.
    gl_Position = vec4(0.0);
}
