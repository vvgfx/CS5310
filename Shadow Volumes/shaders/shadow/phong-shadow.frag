#version 330

struct MaterialProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct LightProperties
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec4 position;
    vec3 spotDirection;   
    float spotAngle;     
};


in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light;
uniform int numLights;

/* texture */
uniform sampler2D image;

out vec4 fColor;
// The only difference between phong-multiple and phong-shadow is that the ambient light is not added to the final color, and the diffuse is 0.8f times the original value.
void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;


    fColor = vec4(0,0,0,1);

    if (light.position.w!=0)
        lightVec = normalize(light.position.xyz - fPosition.xyz);
    else
        lightVec = normalize(-light.position.xyz);

    bool isSpot = light.spotAngle > 0.0;

    //find if point is in spotlight
    bool insideCone = true;
    if(isSpot)
    {
        float theta = dot(normalize(-lightVec), normalize(light.spotDirection));
        insideCone = cos(radians(light.spotAngle)) < theta;
    }
    vec3 tNormal = fNormal;
    normalView = normalize(tNormal.xyz);
    nDotL = dot(normalView,lightVec);

    viewVec = -fPosition.xyz;
    viewVec = normalize(viewVec);

    reflectVec = reflect(-lightVec,normalView);
    reflectVec = normalize(reflectVec);

    rDotV = max(dot(reflectVec,viewVec),0.0);

    ambient = material.ambient * light.ambient;
    diffuse = material.diffuse * light.diffuse * max(nDotL,0) * 0.8f;
    if (nDotL>0)
        specular = material.specular * light.specular * pow(rDotV,material.shininess);
    else
        specular = vec3(0,0,0);

    if(isSpot && insideCone)
        fColor = fColor + vec4(diffuse+specular,1.0);
    else if(!isSpot)
        fColor = fColor + vec4(diffuse+specular,1.0);

    // fColor = texture(image,fTexCoord.st);
    fColor = fColor * texture(image,fTexCoord.st);
    // fColor = vec4(fTexCoord.s,fTexCoord.t,0,1);
}
