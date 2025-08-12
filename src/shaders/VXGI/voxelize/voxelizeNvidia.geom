#version 460 core

#extension GL_NV_geometry_shader_passthrough : enable // -> only on Nvidia GPUs!
// need to use triangles adjacency as the models are imported in that format.
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 3) out;


layout(passthrough) in gl_PerVertex {
    vec4 gl_Position;
} gl_in[];

layout(passthrough) in passThroughData {
    vec4 worldPos;
    vec4 worldNormal;
    vec4 worldTexCoord;
} inData[];


void main()
{
    // since we use triangles_adjacency, the required vertices are 0,2,4;
    vec3 p1 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 p2 = gl_in[4].gl_Position.xyz - gl_in[0].gl_Position.xyz;
    vec3 normal = abs(cross(p1, p2));

    int dominantAxis = normal.y > normal.x ? 1 : 0;
    dominantAxis = normal.z > normal[dominantAxis] ? 2 : dominantAxis;

    gl_ViewportIndex = 2 - dominantAxis;
}
