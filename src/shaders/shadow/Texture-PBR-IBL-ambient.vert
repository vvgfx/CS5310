#version 330

in vec4 vPosition;
in vec4 vNormal;
in vec4 vTexCoord;
in vec4 vTangent;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 texturematrix;
uniform mat4 normalmatrix;

out vec4 fTexCoord;
out vec4 fPosition;
out vec3 fTangent;
out vec3 fBiTangent;
out vec3 fNormal;

void main()
{
    fPosition = model * vec4(vPosition.xyzw);
    gl_Position = projection * model * vPosition;
    fTexCoord = texturematrix * vec4(1*vTexCoord.s,1*vTexCoord.t,0,1);

    vec4 tNormal =  normalmatrix * vNormal;
    fNormal = normalize(tNormal.xyz);

    vec4 tTangent = model * vTangent; 
    fTangent = normalize(tTangent.xyz);

    fBiTangent = cross(fNormal,fTangent);
    fBiTangent = normalize(fBiTangent);
}
