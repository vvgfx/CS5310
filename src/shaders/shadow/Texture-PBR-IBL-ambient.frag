#version 330

in vec4 fPosition;
in vec4 fTexCoord;
in vec3 fTangent;
in vec3 fBiTangent;
in vec3 fNormal;

uniform vec3 cameraPos;  // Camera position in world space

uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

out vec4 fColor;

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
void main()
{

    // Sample textures
    vec3 albedo = pow(texture(albedoMap, fTexCoord.st).rgb, vec3(2.2)); // sRGB to linear
    float metallic = texture(metallicMap, fTexCoord.st).r;
    float roughness = texture(roughnessMap, fTexCoord.st).r;
    float ao = texture(aoMap, fTexCoord.st).r;
    
    // Get normal from normal map
    vec3 normal = texture(normalMap, fTexCoord.st).rgb;
    normal = normal * 2 - 1; 
    normal = normalize(normal);
    
    // Transform normal from tangent space to world space
    vec3 tangent = normalize(fTangent);
    vec3 biTangent = normalize(fBiTangent);
    vec3 tempNormal = normalize(fNormal);
    
    vec3 worldNormal = normalize(
        normal.x * tangent + 
        normal.y * biTangent + 
        normal.z * tempNormal
    );
    
    // View vector in world space
    vec3 viewVec = normalize(cameraPos - fPosition.xyz);
    
    // Reflection vector in world space
    vec3 reflectionVec = reflect(-viewVec, tempNormal);
    
    // PBR calculations
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    
    vec3 F = FresnelSchlick(max(dot(tempNormal, viewVec), 0.0), F0);
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= (1.0 - metallic);
    
    // IBL diffuse - using world space normal
    vec3 irradiance = texture(irradianceMap, tempNormal).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // IBL specular - using world space reflection
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflectionVec, roughness * MAX_REFLECTION_LOD).rgb;
    vec2 brdf = texture(brdfLUT, vec2(max(dot(tempNormal, viewVec), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);
    
    // Combine with AO
    vec3 ambient = (kD * diffuse + specularIBL) * ao;
    
    // Output linear HDR values (NO tone mapping)
    fColor = vec4(ambient, 1.0);
}
