#version 460

#extension GL_NV_gpu_shader5 : enable
#extension GL_NV_shader_atomic_fp16_vector : require


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
    float spotAngleCosine;   // this is the cosine, not the actual angle itself.
};

uniform int numLights;
const int MAXLIGHTS = 10;
uniform LightProperties light[MAXLIGHTS];

layout(binding = 0, rgba16f) uniform image3D ImgResult;

uniform sampler2D albedoMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;

uniform vec4 gridMin;
uniform vec4 gridMax;

ivec3 worldToVoxelSpace(vec3 worldPos);


void main()
{
    vec4 fPosition  = data.worldPos;
    vec4 fTexCoord  = data.worldTexCoord;
    vec3 albedo     = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2));
    vec3 normal     = data.worldNormal.xyz;
    float metallic  = texture(metallicMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float roughness = texture(roughnessMap, vec2(fTexCoord.s,fTexCoord.t)).r;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

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

        dist = length(light[i].position.xyz - fPosition.xyz);
        attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);
        radiance = light[i].color * attenuation;

        nDotL = max(dot(normal, lightVec), 0.0f);
        lightContribution = albedo * (1.0 - metallic) * radiance * nDotL * spotAttenuation;
        Lo += lightContribution;
    }

    vec3 ambient = vec3(0.03f) * albedo;
    vec3 color = ambient + Lo;

    // convert to voxel space here!
    ivec3 voxelPos = worldToVoxelSpace(fPosition.xyz);
    imageAtomicMax(ImgResult, voxelPos, f16vec4(color , 1.0));
    // imageAtomicMax(ImgResult, voxelPos, f16vec4(1.0, 0.0, 0.0, 1.0));

}

ivec3 worldToVoxelSpace(vec3 worldPos)
{
    vec3 uvw = (worldPos - gridMin.xyz) / (gridMax.xyz - gridMin.xyz);
    ivec3 voxelPos = ivec3(uvw * imageSize(ImgResult));
    return voxelPos;
}
