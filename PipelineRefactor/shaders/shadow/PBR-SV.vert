#version 330

in vec4 vPosition;
in vec4 vNormal;

uniform mat4 projection;
uniform mat4 modelview;
uniform mat4 normalmatrix;


out vec4 fPosition;
out vec3 fNormal;

void main()
{

    //gl_Position stuff
    fPosition = modelview * vec4(vPosition.xyzw); // view space
    gl_Position = projection * fPosition;

    //normal transformation stuff
    vec4 tNormal =  normalmatrix * vNormal; // now this should also be in the view space!
    fNormal = normalize(tNormal.xyz);

}
