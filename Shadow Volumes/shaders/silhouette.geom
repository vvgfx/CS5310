layout(triangles_adjacency) in;
layout(line_strip, max_vertices = 6) out;
in vec3 vertPosition[];

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

void EmitLine(int StartIndex, int EndIndex)
{
    gl_Position = gl_in[StartIndex].gl_Position;
    EmitVertex();

    gl_Position = gl_in[EndIndex].gl_Position;
    EmitVertex();

    EndPrimitive();
}

uniform vec3 gLightPos; // need to change this!!!

void main()
{
    vec3 e1 = vertPosition[2] - vertPosition[0];
    vec3 e2 = vertPosition[4] - vertPosition[0];
    vec3 e3 = vertPosition[1] - vertPosition[0];
    vec3 e4 = vertPosition[3] - vertPosition[2];
    vec3 e5 = vertPosition[4] - vertPosition[2];
    vec3 e6 = vertPosition[5] - vertPosition[0];

    vec3 Normal = cross(e1, e2);
    for (int i = 0; i < numLights; i++)
    {
        vec3 gLightPos = light[i].position;
        vec3 LightDir = gLightPos - vertPosition[0];

        if (dot(Normal, LightDir) > 0.00001) {
            Normal = cross(e3, e1);

            if (dot(Normal, LightDir) <= 0) {
                EmitLine(0, 2);
            }

            Normal = cross(e4, e5);
            LightDir = gLightPos - vertPosition[2];

            if (dot(Normal, LightDir) <= 0) {
                EmitLine(2, 4);
            }

            Normal = cross(e2, e6);
            LightDir = gLightPos - vertPosition[4];

            if (dot(Normal, LightDir) <= 0) {
                EmitLine(4, 0);
            }
        }
    }
}
