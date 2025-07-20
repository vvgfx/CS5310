#version 330

in vec4 vPosition;
in vec4 vNormal;
in vec4 vTexCoord;
in vec4 vTangent;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 normalmatrix;
uniform mat4 texturematrix;


out vec4 fPosition;
out vec3 fNormal;
out vec4 fTexCoord;
out vec3 fTangent;
out vec3 fBiTangent;

void main()
{

    //gl_Position stuff
    fPosition = model * vec4(vPosition.xyzw); // moving this to world space instead of view space
    gl_Position = projection * view * fPosition;

    //normal transformation stuff
    vec4 tNormal =  normalmatrix * vNormal; // now this should also be in the world space!
    fNormal = normalize(tNormal.xyz);

    //tangent stuff
    vec4 tTangent = model * vTangent; // this is in the world space! (so cosines should give me world to tangent space transformations)
    fTangent = normalize(tTangent.xyz);
    //calculating the bitangent here instead of the cpu!
    fBiTangent = cross(fNormal,fTangent);
    fBiTangent = normalize(fBiTangent);


    fTexCoord = texturematrix * vec4(1*vTexCoord.s,1*vTexCoord.t,0,1);

}
