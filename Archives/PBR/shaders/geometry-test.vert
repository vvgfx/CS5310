#version 330

in vec4 vPosition;
in vec4 vNormal;
in vec4 vTexCoord;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 texturematrix;

out vec3 gNormal;
out vec4 gPosition;
out vec4 gTexCoord;
out vec4 vertPosition;

void main()
{
    gPosition = modelview * vec4(vPosition.xyzw);
    gl_Position = projection * gPosition;

    vertPosition = gPosition;

    vec4 tNormal = inverse(transpose(modelview)) * vNormal;
    gNormal = normalize(tNormal.xyz);

    gTexCoord = texturematrix * vec4(1*vTexCoord.s,1*vTexCoord.t,0,1);

}
