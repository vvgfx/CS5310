#version 460 core

// #extension GL_NV_geometry_shader_passthrough : enable -> come back to this later!
// need to use triangles adjacency as the models are imported in that format.
layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 3) out;

in passThroughData {
    vec4 worldPos;
    vec4 worldNormal;
    vec4 worldTexCoord;
} inData[];

out passThroughData {
    vec4 worldPos;
    vec4 worldNormal;
    vec4 worldTexCoord;
} outData;


void main()
{
    // since we use triangles_adjacency, the required vertices are 0,2,4;
    vec3 gPos0 = gl_in[0].gl_Position.xyz, gPos1 = gl_in[2].gl_Position.xyz, gPos2 = gl_in[4].gl_Position.xyz;

    vec3 p1 = gPos1 - gPos0;
    vec3 p2 = gPos2 - gPos0;

    // using positions to calculate accurate normals, to prevent smoothing that happens usually in blender!!!!
    vec3 normal = abs(cross(p1,p2));

    int dominantAxis = normal.y > normal.x ? 1 : 0;
    dominantAxis = normal.z > normal[dominantAxis] ? 2 : dominantAxis;

    // send the world data to the fragment shader.
    int indices[3] = {0,2,4};
    for(int i = 0; i < 3; i++) // 0, 2, 4
    {
        int idx = indices[i];

        outData.worldNormal = inData[idx].worldNormal;
        outData.worldPos = inData[idx].worldPos;
        outData.worldTexCoord = inData[idx].worldTexCoord;
        vec3 pos;
        if(dominantAxis == 0)
            pos = gl_in[idx].gl_Position.zyx;
        else if(dominantAxis == 1)
            pos = gl_in[idx].gl_Position.xzy;
        else
            pos = gl_in[idx].gl_Position.xyz; // no need to swizzle if z is already dominant.
        gl_Position = vec4(pos, 1.0);
        EmitVertex();
    }
    EndPrimitive();
}
