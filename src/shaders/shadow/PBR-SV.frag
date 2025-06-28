#version 330

in vec4 fPosition;
in vec3 fNormal;
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
    float spotAngleCosine; // this is the cosine, not the actual angle itself.
};

const float PI = 3.14159265359;

uniform LightProperties light;
uniform MaterialProperties material;
// uniform vec3 cameraPos; // should no longer need this because all calculations are in the view space now, so cameraPos = (0,0,0)

out vec4 fColor;

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
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
    vec3 tempNormal;
    float dist, attenuation;
    vec3 radiance, specular;
    vec3 kS, kD;
    float nDotL;
    float NDF;
    float G;
    vec3 F;

    tempNormal = normalize(fNormal); // view space

    viewVec = normalize(-fPosition.xyz); // view space

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, material.albedo, material.metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0f);
    float spotAttenuation = 1.0f;
    float angle;
    vec3 lightContribution;

    if (light.position.w != 0)
        lightVec = normalize(light.position.xyz - fPosition.xyz); // both in view space
    else
        lightVec = normalize(-light.position.xyz);

    bool isSpot = light.spotAngleCosine < 0.99;

    // spot direction is already in the view co-ordinate space now.

    spotAttenuation = 1.0f; // no attenuation by default
    angle = 1.0f;
    if (isSpot)
    {
        angle = dot(normalize(-lightVec), normalize(light.spotDirection));
        spotAttenuation = 1.0 - (1.0 - angle) * 1.0 / (1.0 - light.spotAngleCosine);
    }

    spotAttenuation = clamp(spotAttenuation, 0.0f, 1.0f);
    
    halfwayVec = normalize(viewVec + lightVec); // world space

    dist = length(light.position.xyz - fPosition.xyz);
    // attenuation = 1.0 / (dist * dist);
    attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
    radiance = light.color * attenuation;

    // Cook torrance BRDF
    NDF = DistributionGGX(tempNormal, halfwayVec, material.roughness);
    G = GeometrySmith(tempNormal, viewVec, lightVec, material.roughness);
    F = FresnelSchlick(clamp(dot(halfwayVec, viewVec), 0.0f, 1.0f), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(tempNormal, viewVec), 0.0f) * max(dot(tempNormal, lightVec), 0.0f) + 0.001;
    specular = numerator / denominator;

    kS = F; // specular coefficient is equal to fresnel
    kD = vec3(1.0f) - kS;
    kD *= 1.0 - material.metallic;

    nDotL = max(dot(tempNormal, lightVec), 0.0f);

    lightContribution = (kD * material.albedo / PI + specular) * radiance * nDotL * spotAttenuation;
    Lo += lightContribution;

    // ambient lighting is not used here because it runs on a separate pass
    // vec3 ambient = vec3(0.03f) * material.albedo * material.ao;

    // vec3 color = ambient + Lo;

    // HDR tonemapping
    // color = color / (color + vec3(1.0));
    // // gamma correct
    // color = pow(color, vec3(1.0/2.2));

    fColor = vec4(Lo, 1.0);

    // Debugging code here

    // specular = (NDF * G * F) / (4.0 * max(dot(tempNormal, viewVec), 0.0) * max(dot(tempNormal, lightVec), 0.0) + 0.001);
    // fColor = vec4(vec3(light[0].spotDirection.x + 0.5, light[0].spotDirection.y + 0.5, light[0].spotDirection.z + 0.5), 1.0);
    // fColor = vec4(lightContribution, 1.0);
}
