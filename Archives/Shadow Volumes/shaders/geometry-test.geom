#version 330

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 6) out;

in vec4 vertPosition[];
in vec3 gNormal[];
in vec4 gPosition[];
in vec4 gTexCoord[];

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec3 spotDirection;
    float spotAngle;
};

const int MAXLIGHTS = 10;
uniform int numLights;
uniform LightProperties light[MAXLIGHTS];

out vec3 fNormal;
out vec4 fPosition;
out vec4 fTexCoord;

void EmitLine(int StartIndex, int EndIndex)
{
    gl_Position = gl_in[StartIndex].gl_Position;
    // Pass-through all vertex attributes to FS
    fNormal = gNormal[StartIndex];
    fPosition = gPosition[StartIndex];
    fTexCoord = gTexCoord[StartIndex];
    EmitVertex();
    
    gl_Position = gl_in[EndIndex].gl_Position;
    // Pass-through all vertex attributes to FS
    fNormal = gNormal[EndIndex];
    fPosition = gPosition[EndIndex];
    fTexCoord = gTexCoord[EndIndex];
    EmitVertex();
    
    EndPrimitive();
}

void EmitTriangle(int vert1, int vert2, int vert3)
{
    gl_Position = gl_in[vert1].gl_Position;
    fNormal = gNormal[vert1];
    fPosition = gPosition[vert1];
    fTexCoord = gTexCoord[vert1];
    EmitVertex();

    gl_Position = gl_in[vert2].gl_Position;
    fNormal = gNormal[vert2];
    fPosition = gPosition[vert2];
    fTexCoord = gTexCoord[vert2];
    EmitVertex();

    gl_Position = gl_in[vert3].gl_Position;
    fNormal = gNormal[vert3];
    fPosition = gPosition[vert3];
    fTexCoord = gTexCoord[vert3];
    EmitVertex();

}

// uniform vec3 gLightPos; // need to change this!!!

void main()
{
    EmitTriangle(0,2,4);
}
