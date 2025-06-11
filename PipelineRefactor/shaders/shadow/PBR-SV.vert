#version 330

in vec4 vPosition;
in vec4 vNormal;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 normalmatrix;


out vec4 fPosition;
out vec3 fNormal;

void main()
{

    //gl_Position stuff
    fPosition = model * vec4(vPosition.xyzw); // moving this to world space instead of view space
    gl_Position = projection * view * fPosition;

    //normal transformation stuff
    vec4 tNormal =  normalmatrix * vNormal; // now this should also be in the world space!
    fNormal = normalize(tNormal.xyz);

}
