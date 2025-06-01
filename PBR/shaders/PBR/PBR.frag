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
in vec3 fTangent;
in vec3 fBiTangent;

const int MAXLIGHTS = 10;

uniform MaterialProperties material;
uniform LightProperties light;
uniform int numLights;

/* texture */
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
// uniform sampler2D metallicMap;
// uniform sampler2D roughnessMap;
// uniform sampler2D aoMap;
uniform bool PBR; // Using this because the "default" texture seems to have errors
out vec4 fColor;

//This shader supports (and expects) all the PBR-required textureMaps (albedo, normal, metallic, roughness, Ambient Occlusion)
void main()
{
    vec3 lightVec,viewVec,reflectVec;
    vec3 normalView;
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;
    vec3 tempNormal,tempTangent,tempBitangent;
    vec3 tNormal;

    // bilinear interpolation does not guarantee normal vectors stay normal.
    tempNormal = normalize(fNormal);
    tempTangent = normalize(fTangent);
    tempBitangent = normalize(fBiTangent);


    viewVec = -fPosition.xyz;
    viewVec = normalize(viewVec);


    // Idk how this works, but it converts the viewVec to tangent space.
    viewVec = vec3(dot(viewVec,tempTangent),dot(viewVec,tempBitangent),dot(viewVec,tempNormal));
    viewVec = normalize(viewVec);


    // bump mapping texture lookup.
    if(PBR)
    {
        tNormal = texture(normalMap,vec2(fTexCoord.s,fTexCoord.t)).rgb;
        tNormal = 2* tNormal - 1; // [0,1] to [-1,1]
        tNormal = normalize(tNormal);
    }
    else
        tNormal = vec3(0,0,1); // test - this works fine.

    fColor = vec4(0,0,0,1);

    if (light.position.w!=0)
        lightVec = normalize(light.position.xyz - fPosition.xyz);
    else
        lightVec = normalize(-light.position.xyz);
    
    // do the same tangent space transformation for the lightVec
    lightVec = vec3(dot(lightVec,tempTangent),dot(lightVec,tempBitangent),dot(lightVec,tempNormal));
    lightVec = normalize(lightVec);


    bool isSpot = light.spotAngle > 0.0;

    // light is in the view coordinate space. This create an equivalent direction in the tangent space.
    vec3 tangLightSpotDir = vec3(dot(light.spotDirection,tempTangent),dot(light.spotDirection,tempBitangent),dot(light.spotDirection,tempNormal));
    //find if point is in spotlight
    bool insideCone = true;
    if(isSpot)
    {
        float theta = dot(normalize(-lightVec), normalize(tangLightSpotDir));
        insideCone = cos(radians(light.spotAngle)) < theta;
    }


    nDotL = dot(tNormal,lightVec);

    


    reflectVec = reflect(-lightVec,tNormal);
    reflectVec = normalize(reflectVec);

    rDotV = max(dot(reflectVec,viewVec),0.0);

    diffuse = material.diffuse * light.diffuse * max(nDotL,0) * 0.8;
    if (nDotL>0)
        specular = material.specular * light.specular * pow(rDotV,material.shininess);
    else
        specular = vec3(0,0,0);

    if(isSpot && insideCone)
        fColor = fColor + vec4(diffuse+specular,1.0);
    else if(!isSpot)
        fColor = fColor + vec4(diffuse+specular,1.0);

    
    fColor = fColor * texture(albedoMap,fTexCoord.st);
    
}
