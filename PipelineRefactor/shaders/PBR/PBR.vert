in vec4 vPosition;
in vec4 vNormal;
in vec4 vTexCoord;
in vec4 vTangent;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalmatrix;
uniform mat4 texturematrix;

out vec4 fPosition;
out vec4 fTexCoord;
out vec3 fNormal;

void main()
{

    //gl_Position stuff
    fPosition = modelview * vec4(vPosition.xyzw);
    gl_Position = projection * fPosition;

    //normal transformation stuff
    vec4 tNormal =  normalMatrix * vNormal;
    fNormal = normalize(tNormal.xyz);

    fTexCoord = texturematrix * vec4(1*vTexCoord.s,1*vTexCoord.t,0,1);
}
