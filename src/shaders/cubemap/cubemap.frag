#version 330


in vec3 texCoord;
out vec4 fColor;

uniform samplerCube skybox;

void main()
{    
    fColor = texture(skybox, texCoord);
}
