#version 460

// Atomic voxelization into 3 r32ui via floatBitsToUint (positive floats compare
// monotonically as uints). R is also a "written this frame" breadcrumb.

in passThroughData {
    vec4 worldPos;
    vec4 worldNormal;
    vec4 worldTexCoord;
} data;

struct LightProperties
{
    vec4 position;
    vec3 color;
    vec3 spotDirection;
    float spotAngleCosine;
};

uniform int numLights;
const int MAXLIGHTS = 10;
uniform LightProperties light[MAXLIGHTS];

layout(binding = 0, r32ui) restrict uniform uimage3D ImgResultR;
layout(binding = 1, r32ui) restrict uniform uimage3D ImgResultG;
layout(binding = 2, r32ui) restrict uniform uimage3D ImgResultB;

uniform sampler2D albedoMap;

uniform vec4 gridMin;
uniform vec4 gridMax;

ivec3 worldToVoxelSpace(vec3 worldPos);

void main()
{
    vec4 fPosition  = data.worldPos;
    vec4 fTexCoord  = data.worldTexCoord;
    vec3 albedo     = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2));
    vec3 normal     = data.worldNormal.xyz;

    float spotAttenuation = 1.0f;
    float angle, dist, attenuation, nDotL;

    vec3 lightVec, radiance, lightContribution;
    vec3 Lo = vec3(0.0f);
    for(int i = 0; i < numLights; i++)
    {
         if (light[i].position.w!=0)
            lightVec = normalize(light[i].position.xyz - fPosition.xyz);
        else
            lightVec = normalize(-light[i].position.xyz);

        bool isSpot = light[i].spotAngleCosine < 0.95;
        angle = 1.0f;
        if(isSpot)
        {
            angle = dot(normalize(-lightVec), normalize(light[i].spotDirection));
            spotAttenuation = 1.0 - (1.0 - angle) * 1.0/(1.0 - light[i].spotAngleCosine);
        }

        spotAttenuation = clamp(spotAttenuation, 0.0f, 1.0f);

        nDotL = max(dot(normal, lightVec), 0.0f);
        dist = length(light[i].position.xyz - fPosition.xyz);
        attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        radiance = light[i].color * attenuation;

        lightContribution = albedo * radiance * nDotL * spotAttenuation;
        Lo += lightContribution;
    }

    vec3 color = max(Lo, vec3(0.0));

    ivec3 voxelPos = worldToVoxelSpace(fPosition.xyz);

    // Breadcrumb: written voxels have R >= 1 even if lighting is zero.
    const uint breadcrumb = 1u;
    imageAtomicMax(ImgResultR, voxelPos, max(floatBitsToUint(color.r), breadcrumb));
    imageAtomicMax(ImgResultG, voxelPos, floatBitsToUint(color.g));
    imageAtomicMax(ImgResultB, voxelPos, floatBitsToUint(color.b));
}

ivec3 worldToVoxelSpace(vec3 worldPos)
{
    vec3 uvw = (worldPos - gridMin.xyz) / (gridMax.xyz - gridMin.xyz);
    ivec3 voxelPos = ivec3(uvw * imageSize(ImgResultR));
    return voxelPos;
}
