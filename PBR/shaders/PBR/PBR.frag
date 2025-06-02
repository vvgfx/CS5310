#version 330


struct LightProperties
{
    // vec3 ambient;
    // vec3 diffuse;
    // vec3 specular;
    vec4 position;
    vec3 spotDirection;   
    float spotAngle;
    vec3 color; // need to pass this
};


in vec3 fNormal;
in vec4 fPosition;
in vec4 fTexCoord;
in vec3 fTangent;
in vec3 fBiTangent;

const int MAXLIGHTS = 10;

// uniform MaterialProperties material;
uniform LightProperties light;
uniform int numLights;

/* texture */
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform bool PBR; // Using this because the "default" texture seems to have errors
out vec4 fColor;


const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}


//This shader supports (and expects) all the PBR-required textureMaps (albedo, normal, metallic, roughness, Ambient Occlusion)
void main()
{
    vec3 lightVec,viewVec,reflectVec,halfwayVec; // using halfway vector instead of reflection vector for PBR
    vec3 ambient,diffuse,specular;
    float nDotL,rDotV;
    vec3 tempNormal,tempTangent,tempBitangent;
    vec3 tNormal;

    // bump mapping texture lookup.
    // if(PBR)
    // {
        tNormal = texture(normalMap,vec2(fTexCoord.s,fTexCoord.t)).rgb;
        tNormal = 2* tNormal - 1; // [0,1] to [-1,1]
        tNormal = normalize(tNormal);
    // }
    // else
    //     tNormal = vec3(0,0,1); // test - this works fine.


    // GET THE REQUIRED MATERIAL VALUES FROM THE INPUT TEXTURES --------------------------------------------------

    vec3 albedo     = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2)); // conversion from sRGB to linear space
    vec3 normal     = tNormal;
    float metallic  = texture(metallicMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float roughness = texture(roughnessMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float ao        = texture(aoMap, vec2(fTexCoord.s,fTexCoord.t)).r;

    // END MATERIAL RETRIVAL -------------------------------------------------------------------------------------


    //CONVERSION TO TANGENT SPACE STARTS HERE ---------------------------------------------------------------------

    // bilinear interpolation does not guarantee normal vectors stay normal.
    tempNormal = normalize(fNormal);
    tempTangent = normalize(fTangent);
    tempBitangent = normalize(fBiTangent);


    viewVec = -fPosition.xyz; // since this is in the view co-ordinate system at this line, the camera is at 0,0,0. So 0 - fPosition = -fPosition
    viewVec = normalize(viewVec);


    // Idk how this works, but it converts the viewVec to tangent space.
    viewVec = vec3(dot(viewVec,tempTangent),dot(viewVec,tempBitangent),dot(viewVec,tempNormal));
    viewVec = normalize(viewVec);

    fColor = vec4(0,0,0,1);

    if (light.position.w!=0)
        lightVec = normalize(light.position.xyz - fPosition.xyz);
    else
        lightVec = normalize(-light.position.xyz);
    
    // do the same tangent space transformation for the lightVec
    lightVec = vec3(dot(lightVec,tempTangent),dot(lightVec,tempBitangent),dot(lightVec,tempNormal));
    lightVec = normalize(lightVec);

    // light is in the view coordinate space. This create an equivalent direction in the tangent space.
    vec3 tangLightSpotDir = vec3(dot(light.spotDirection,tempTangent),dot(light.spotDirection,tempBitangent),dot(light.spotDirection,tempNormal));
    bool isSpot = light.spotAngle > 0.0;

    // END CONVERSION TO TANGENT SPACE -------------------------------------------------------------------------------
    
    //find if point is in spotlight - this is ignored for now. Need to come back later to reimplement this.
    bool insideCone = true;
    if(isSpot)
    {
        float theta = dot(normalize(-lightVec), normalize(tangLightSpotDir));
        insideCone = cos(radians(light.spotAngle)) < theta;
    }

    vec3 Lo = vec3(0.0);

    halfwayVec = normalize(viewVec + lightVec);
    float distance = length(light.position.xyz - fPosition.xyz); // distance should be the same regardless of what coordinate system beacuse it's just a magnitude (hopefully)
    float attenuation = 1.0 / (distance * distance);
    vec3 radiance = light.color * attenuation;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    vec3 F  = fresnelSchlick(max(dot(halfwayVec, viewVec), 0.0), F0); // fresnel
    float NDF = DistributionGGX(tNormal, halfwayVec, roughness); // ggx normal distribution function
    float G   = GeometrySmith(tNormal, viewVec, lightVec, roughness);  // geometry function

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(tNormal, viewVec), 0.0) * max(dot(tNormal, lightVec), 0.0)  + 0.0001; // the 0.0001 is for safety in case the denominator is 0
    specular     = numerator / denominator;  

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	// This nullifies the diffuse part if the material is metallic!

    nDotL = max(dot(tNormal, lightVec), 0.0);        
    Lo += (kD * albedo / PI + specular) * radiance * nDotL;

    ambient = vec3(0.3) * albedo * ao;
    vec3 color = ambient + Lo; 

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2));  

    fColor = vec4(color, 1.0f);
}
