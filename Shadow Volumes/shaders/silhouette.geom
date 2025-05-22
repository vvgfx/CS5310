#version 330

layout (triangles_adjacency) in;
layout (line_strip, max_vertices = 6) out;

in vec3 gPosition[];
void EmitLine(int StartIndex, int EndIndex)
{
    gl_Position = gl_in[StartIndex].gl_Position;
    EmitVertex();

    gl_Position = gl_in[EndIndex].gl_Position;
    EmitVertex();

    EndPrimitive();
}
uniform vec3 gLightPos; // Pass this through the shadowRenderer
void main()
{
    vec3 e1 = gPosition[2] - gPosition[0];
    vec3 e2 = gPosition[4] - gPosition[0];
    vec3 e3 = gPosition[1] - gPosition[0];
    vec3 e4 = gPosition[3] - gPosition[2];
    vec3 e5 = gPosition[4] - gPosition[2];
    vec3 e6 = gPosition[5] - gPosition[0];

    vec3 Normal = cross(e1,e2);
    vec3 LightDir = gLightPos - gPosition[0];

    if (dot(Normal, LightDir) > 0.00001) {

        Normal = cross(e3,e1);

        if (dot(Normal, LightDir) <= 0) {
            EmitLine(0, 2);
        }

        Normal = cross(e4,e5);
        LightDir = gLightPos - gPosition[2];

        if (dot(Normal, LightDir) <=0) {
            EmitLine(2, 4);
        }

        Normal = cross(e2,e6);
        LightDir = gLightPos - gPosition[4];

        if (dot(Normal, LightDir) <= 0) {
            EmitLine(4, 0);
        }
    }
}
