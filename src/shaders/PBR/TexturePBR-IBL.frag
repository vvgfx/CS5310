#version 330

in vec4 fPosition;
in vec3 fNormal;
in vec4 fTexCoord;
// These transform from the world space to the tangent space.
in vec3 fTangent;
in vec3 fBiTangent;

precision highp float;
struct MaterialProperties
{
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

struct LightProperties
{
    vec4 position;
    vec3 color;
    vec3 spotDirection;   
    float spotAngleCosine;   
};

const int MAXLIGHTS = 10;
const float PI = 3.14159265359;

uniform int numLights;
uniform LightProperties light[MAXLIGHTS];
uniform MaterialProperties material;
uniform vec3 cameraPos; // need this because I'm moving all calculations to the world space

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// adding the irradiance IBL here
uniform samplerCube irradianceMap;

out vec4 fColor;

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
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

void main()
{
    vec3 viewVec, lightVec, halfwayVec;
    vec3 tempNormal, tempTangent, tempBiTangent, tNormal;
    float dist, attenuation;
    vec3 radiance, specular;
    vec3 kS, kD;
    float nDotL;
    float NDF;
    float G;
    vec3 F;

    tempNormal = normalize(fNormal); // world space -> this world space equivalent is required to 

    // normalize the tangent and bitangents because of bilinear interpolation.
    tempTangent = normalize(fTangent);
    tempBiTangent = normalize(fBiTangent);

    viewVec = normalize(cameraPos - fPosition.xyz); // world space
    // transform the viewVec to the tangent space
    viewVec = vec3(dot(viewVec,tempTangent),dot(viewVec,tempBiTangent),dot(viewVec,tempNormal));
    viewVec = normalize(viewVec);

    // get normal coordinates in tangent space
    tNormal = texture(normalMap,vec2(fTexCoord.s,fTexCoord.t)).rgb;
    tNormal = 2* tNormal - 1; // [0,1] to [-1,1]
    tNormal = normalize(tNormal);

    // getting required values from the input textures
    vec3 albedo     = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2)); // conversion from sRGB to linear space
    // vec3 albedo     = texture(albedoMap, fTexCoord.st).rgb; // turns out the albedo was in srgb space, needed to convert to linear, so that the tonemapping converts it back to srgb later.
    vec3 normal     = tNormal;
    float metallic  = texture(metallicMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float roughness = texture(roughnessMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float ao        = texture(aoMap, vec2(fTexCoord.s,fTexCoord.t)).r; // check if you have to convert from sRGB to linear space as well.




    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0f);

    // spotlight stuff
    float spotAttenuation = 1.0f;
    float angle;
    for(int i = 0; i < numLights; i++)
    {
        if (light[i].position.w!=0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz); // both in world space
        else
            lightVec = normalize(-light[i].position.xyz);

        // transform the lightVec to the tangent space
        lightVec = vec3(dot(lightVec,tempTangent),dot(lightVec,tempBiTangent),dot(lightVec,tempNormal));

        bool isSpot = light[i].spotAngleCosine < 0.95;

        spotAttenuation = 1.0f; // no attenuation by default

        if(isSpot)
        {
            vec3 spotDirTangent = vec3(dot(light[i].spotDirection,tempTangent),dot(light[i].spotDirection,tempBiTangent),dot(light[i].spotDirection,tempNormal));
            angle = dot(normalize(-lightVec), normalize(spotDirTangent));
            spotAttenuation = 1.0 - (1.0 - angle) * 1.0/(1.0 - light[i].spotAngleCosine);
        }

        spotAttenuation = clamp(spotAttenuation, 0.0f, 1.0f);

        halfwayVec = normalize(viewVec + lightVec); // already in tangent space

        // for the distance, I.m converting to tangent space first and then taking the distance. 
        // I don't think this is required because dist is in world space, and tangent space does not have a scale,
        // but doing this for safety anyways.
        vec3 distVec = light[i].position.xyz - fPosition.xyz;
        distVec = vec3(dot(distVec,tempTangent),dot(distVec,tempBiTangent),dot(distVec,tempNormal));
        dist = length(distVec);
        // attenuation = 1.0 / (dist * dist);
        attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist); // constant linear quadratic attenuation
        radiance = light[i].color * attenuation;


        // Cook torrance BRDF
        NDF = DistributionGGX(normal, halfwayVec, roughness);
        G = GeometrySmith(normal, viewVec, lightVec, roughness);
        F = FresnelSchlick(clamp(dot(halfwayVec, viewVec), 0.0f, 1.0f), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(normal, viewVec), 0.0f) * max(dot(normal, lightVec), 0.0f) + 0.001;
        specular = numerator/denominator;

        kS = F; // specular coefficient is equal to fresnel
        kD = vec3(1.0f) - kS;
        kD *= 1.0 - metallic; 

        nDotL = max(dot(normal, lightVec), 0.0f);

        Lo += (kD * albedo / PI + specular) * radiance * nDotL * spotAttenuation;

    }

    // instead of using albedo for the ambient, we are using the irradiance map now.
    // vec3 ambient = vec3(0.03f) * albedo * ao;
    kS = FresnelSchlick(max(dot(normal, viewVec), 0.0), F0); // both in tangent space
    kD = 1.0 - kS;
    kD *= (1.0 - metallic);
    vec3 irradiance = texture(irradianceMap, tempNormal).rgb; // need this in world space, so using tempNormal!
    vec3 diffuse = irradiance * albedo;
    vec3 ambient = (kD * diffuse) * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    fColor = vec4(color, 1.0);
}
