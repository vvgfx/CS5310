#version 330


in vec3 texCoord;
out vec4 fColor;

uniform samplerCube skybox;

void main()
{    
    fColor = vec4(pow(texture(skybox, texCoord).rgb,vec3(2.2)), 1.0); // just converting from srgb to linear space because post-processing pass will convert to srgb again.
}
