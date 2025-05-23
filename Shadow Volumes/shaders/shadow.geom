#version 330

layout(triangles_adjacency) in;
layout(triangle_strip, max_vertices = 18) out;

in vec3 gPosition[];

uniform mat4 projection; // these should share the memory with the vertex shader?
uniform mat4 modelview;

void EmitLine(int StartIndex, int EndIndex)
{
    gl_Position = gl_in[StartIndex].gl_Position;
    EmitVertex();

    gl_Position = gl_in[EndIndex].gl_Position;
    EmitVertex();

    EndPrimitive();
}

uniform vec3 gLightPos; // Pass this through the shadowRenderer

float EPSILON = 0.0001;
// Emit a quad using a triangle strip
void EmitQuad(vec3 StartVertex, vec3 EndVertex)
{
    // Vertex #1: the starting vertex (just a tiny bit below the original edge)
    vec3 LightDir = normalize(StartVertex - gLightPos);
    mat4 wvp = projection;
    gl_Position = wvp * vec4((StartVertex + LightDir * EPSILON), 1.0);
    EmitVertex();

    // Vertex #2: the starting vertex projected to infinity
    gl_Position = wvp * vec4(LightDir, 0.0);
    EmitVertex();

    // Vertex #3: the ending vertex (just a tiny bit below the original edge)
    LightDir = normalize(EndVertex - gLightPos);
    gl_Position = wvp * vec4((EndVertex + LightDir * EPSILON), 1.0);
    EmitVertex();

    // Vertex #4: the ending vertex projected to infinity
    gl_Position = wvp * vec4(LightDir, 0.0);
    EmitVertex();

    EndPrimitive();
}

void main()
{
    vec3 e1 = gPosition[2] - gPosition[0];
    vec3 e2 = gPosition[4] - gPosition[0];
    vec3 e3 = gPosition[1] - gPosition[0];
    vec3 e4 = gPosition[3] - gPosition[2];
    vec3 e5 = gPosition[4] - gPosition[2];
    vec3 e6 = gPosition[5] - gPosition[0];

    vec3 Normal = cross(e1, e2);
    vec3 LightDir = gLightPos - gPosition[0];

    // Handle only light facing triangles
    if (dot(Normal, LightDir) > 0) {
        Normal = cross(e3, e1);

        if (dot(Normal, LightDir) <= 0) {
            vec3 StartVertex = gPosition[0];
            vec3 EndVertex = gPosition[2];
            EmitQuad(StartVertex, EndVertex);
        }

        Normal = cross(e4, e5);
        LightDir = gLightPos - gPosition[2];

        if (dot(Normal, LightDir) <= 0) {
            vec3 StartVertex = gPosition[2];
            vec3 EndVertex = gPosition[4];
            EmitQuad(StartVertex, EndVertex);
        }

        Normal = cross(e2, e6);
        LightDir = gLightPos - gPosition[4];

        if (dot(Normal, LightDir) <= 0) {
            vec3 StartVertex = gPosition[4];
            vec3 EndVertex = gPosition[0];
            EmitQuad(StartVertex, EndVertex);
        }
    }
}
