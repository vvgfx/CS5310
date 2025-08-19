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
    vec3 viewVec, lightVec, tangentHalfwayVec;
    vec3 worldNormal, worldTangent, worldBiTangent, tNormal;
    float dist, attenuation;
    vec3 radiance, specular;
    vec3 kS, kD;
    float nDotL;
    float NDF;
    float G;
    vec3 F;

    worldNormal = normalize(fNormal);
    worldTangent = normalize(fTangent);
    worldBiTangent = normalize(fBiTangent);

    viewVec = normalize(cameraPos - fPosition.xyz);
    
    // transform the viewVec to the tangent space
    vec3 tangentViewVec = vec3(dot(viewVec,worldTangent),dot(viewVec,worldBiTangent),dot(viewVec,worldNormal));
    tangentViewVec = normalize(tangentViewVec);

    // get normal coordinates in tangent space
    tNormal = texture(normalMap,vec2(fTexCoord.s,fTexCoord.t)).rgb;
    tNormal = 2* tNormal - 1; // [0,1] to [-1,1]
    tNormal = normalize(tNormal);

    // getting required values from the input textures
    vec3 albedo         = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2)); // conversion from sRGB to linear space
    vec3 tangentNormal  = tNormal;
    float metallic      = texture(metallicMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float roughness     = texture(roughnessMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float ao            = texture(aoMap, vec2(fTexCoord.s,fTexCoord.t)).r;

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
        vec3 tangentLightvec = vec3(dot(lightVec,worldTangent),dot(lightVec,worldBiTangent),dot(lightVec,worldNormal));

        bool isSpot = light[i].spotAngleCosine < 0.95;

        spotAttenuation = 1.0f; // no attenuation by default

        if(isSpot)
        {
            vec3 tangentSpotDir = vec3(dot(light[i].spotDirection,worldTangent),dot(light[i].spotDirection,worldBiTangent),dot(light[i].spotDirection,worldNormal));
            angle = dot(normalize(-tangentLightvec), normalize(tangentSpotDir));
            spotAttenuation = 1.0 - (1.0 - angle) * 1.0/(1.0 - light[i].spotAngleCosine);
        }

        spotAttenuation = clamp(spotAttenuation, 0.0f, 1.0f);

        tangentHalfwayVec = normalize(tangentViewVec + tangentLightvec);

        // for the distance, I.m converting to tangent space first and then taking the distance. 
        // I don't think this is required.
        vec3 distVec = light[i].position.xyz - fPosition.xyz;
        vec3 tangentDistVec = vec3(dot(distVec,worldTangent),dot(distVec,worldBiTangent),dot(distVec,worldNormal));
        dist = length(tangentDistVec);
        attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist); // constant linear quadratic attenuation
        radiance = light[i].color * attenuation;

        // Cook torrance BRDF
        NDF = DistributionGGX(tangentNormal, tangentHalfwayVec, roughness);
        G = GeometrySmith(tangentNormal, tangentViewVec, tangentLightvec, roughness);
        F = FresnelSchlick(clamp(dot(tangentHalfwayVec, tangentViewVec), 0.0f, 1.0f), F0);

        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(tangentNormal, tangentViewVec), 0.0f) * max(dot(tangentNormal, tangentLightvec), 0.0f) + 0.001;
        specular = numerator/denominator;

        kS = F; // specular coefficient is equal to fresnel
        kD = vec3(1.0f) - kS;
        kD *= 1.0 - metallic; 

        nDotL = max(dot(tangentNormal, tangentLightvec), 0.0f);

        Lo += (kD * albedo / PI + specular) * radiance * nDotL * spotAttenuation;

    }

    // ambient lighting here
    vec3 ambient = vec3(0.03f) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    fColor = vec4(color, 1.0);
}
