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

// VXGI
uniform sampler3D voxelTexture;
uniform vec4 gridMin;
uniform vec4 gridMax;
uniform float voxelResolution;
uniform int useGI;

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
// ----------------------------------------------------------------------------
// VXGI Methods here.

const float CONE_SPREAD = 0.577; // tan(30) for 60 cone
const int NUM_CONES = 6;
const float MAX_TRACE_DISTANCE = 50.0;
const vec3 CONE_DIRECTIONS[6] = vec3[](
    vec3( 0.0,  1.0,  0.0),
    vec3( 0.866,  0.5,  0.0),
    vec3( 0.267,  0.5,  0.823),
    vec3(-0.700,  0.5,  0.509),
    vec3(-0.700,  0.5, -0.509),
    vec3( 0.267,  0.5, -0.823)
);

vec3 worldToUVW(vec3 worldPos) 
{
    return (worldPos - gridMin.xyz) / (gridMax.xyz - gridMin.xyz);
}

vec4 traceCone(vec3 origin, vec3 direction, float tanHalfAngle) 
{
    vec4 color = vec4(0.0);
    float voxelSize = (gridMax.x - gridMin.x) / voxelResolution;

    float dist = voxelSize * 2.0;  // prevent self-occlusion

    while (dist < MAX_TRACE_DISTANCE && color.a < 0.95) 
    {
        vec3 samplePos = origin + direction * dist;
        vec3 uvw = worldToUVW(samplePos);

        // exit criteria for outside of grid.
        if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
            break;

        float diameter = 2.0 * tanHalfAngle * dist;
        float mipLevel = log2(max(1.0, diameter / voxelSize));

        vec4 voxelSample = textureLod(voxelTexture, uvw, mipLevel);

        // Front-to-back accumulation
        float a = 1.0 - color.a;
        color.rgb += a * voxelSample.rgb;
        color.a += a * voxelSample.a;

        dist += max(voxelSize, diameter * 0.5);
    }

    return color;
}

vec3 calculateGI(vec3 worldPos, vec3 normal)
{
    vec3 indirectLight = vec3(0.0);
    float totalWeight = 0.0;

    vec3 origin = worldPos + normal * (2.0 / voxelResolution);
    vec3 up = abs(normal.y) < 0.999 ? vec3(0, 1, 0) : vec3(1, 0, 0);
    vec3 tangent = normalize(cross(up, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, normal, bitangent);

    for (int i = 0; i < NUM_CONES; i++) 
    {
        vec3 coneDir = normalize(TBN * CONE_DIRECTIONS[i]);

        // flip direction to make sure the cone is in the proper orientation.
        if (dot(normal, coneDir) < 0.0)
            coneDir = -coneDir;

        float NdotL = dot(normal, coneDir);

        vec4 result = traceCone(origin, coneDir, CONE_SPREAD);
        indirectLight += result.rgb * NdotL;
        totalWeight += NdotL;
    }

    if (totalWeight > 0.0)
        indirectLight /= totalWeight;
    
    return indirectLight;
}

float traceOcclusion(vec3 origin, vec3 lightDir, float maxDist) 
{
    float occlusion = 0.0;
    float voxelSize = (gridMax.x - gridMin.x) / voxelResolution;
    float dist = voxelSize * 4.0;  // Start offset
    
    // Narrower cone for shadows (5-10° vs 60° for GI)
    float tanHalfAngle = 0.0875;  // tan(5°) for sharper shadows
    
    while (dist < maxDist && occlusion < 1.0) 
    {
        vec3 pos = origin + lightDir * dist;
        vec3 uvw = worldToUVW(pos);
        
        if (any(lessThan(uvw, vec3(0.0))) || any(greaterThan(uvw, vec3(1.0))))
            break;
        
        float diameter = 2.0 * tanHalfAngle * dist;
        float mipLevel = log2(max(1.0, diameter / voxelSize));
        
        float alpha = textureLod(voxelTexture, uvw, mipLevel).a;
        
        // Accumulate occlusion
        occlusion += (1.0 - occlusion) * alpha;
        
        dist += max(voxelSize, diameter * 0.5);
    }
    
    return 1.0 - occlusion;  // Return visibility (0=blocked, 1=visible)
}

vec3 traceReflection(vec3 origin, vec3 normal, vec3 viewDir, float roughness)
{
    vec3 reflectDir = reflect(-viewDir, normal);
    
    // Wider cone for rougher surfaces
    float tanHalfAngle = mix(0.0, CONE_SPREAD, roughness);
    
    vec4 result = traceCone(
        origin + normal * (2.0 / voxelResolution),
        reflectDir,
        tanHalfAngle
    );
    
    return result.rgb;
}


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

    mat3 TBN = mat3(normalize(fTangent), normalize(fBiTangent), normalize(fNormal));
    vec3 wNormal = normalize(TBN * tangentNormal);

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

        // vx occlusion
        float visibility = 1.0;
        if(useGI > 0)
        {
            vec3 lightDir = normalize(light[i].position.xyz - fPosition.xyz);
            float lightDist = distance(light[i].position.xyz, fPosition.xyz);
            visibility = traceOcclusion(
                fPosition.xyz + wNormal * (2.0 / voxelResolution),
                lightDir,
                lightDist
            );
        }


        Lo += (kD * albedo / PI + specular) * radiance * nDotL * spotAttenuation * visibility;

    }

    // ambient lighting here
    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 indirectDiffuse = vec3(0.0);
    vec3 indirectSpecular = vec3(0.0);
    if(useGI > 0)
    {
        indirectDiffuse = calculateGI(fPosition.xyz, wNormal);      // Once
        indirectSpecular = traceReflection(fPosition.xyz, wNormal, viewVec, roughness);         // Once
    }
    
    F = FresnelSchlick(max(dot(worldNormal, viewVec), 0.0), F0);
    kS = F;
    kD = (1.0 - kS) * (1.0 - metallic);

    vec3 color = ambient + Lo + kD * indirectDiffuse * albedo + kS * indirectSpecular;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    fColor = vec4(color, 1.0);
}
