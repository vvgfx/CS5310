#version 330

in vec4 fPosition;
in vec4 fTexCoord;

// conversion to tangent space :)
in vec3 fTangent;
in vec3 fBiTangent;
in vec3 fNormal;

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
    // all calculations here take place in the world space.
    vec3 viewVec, tangent, biTangent, normal, tempNormal;
    vec3 kS, kD;

    vec3 albedo     = pow(texture(albedoMap, vec2(fTexCoord.s,fTexCoord.t)).rgb, vec3(2.2)); // sRGB!!
    float metallic  = texture(metallicMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float roughness = texture(roughnessMap, vec2(fTexCoord.s,fTexCoord.t)).r;
    float ao        = texture(aoMap, vec2(fTexCoord.s,fTexCoord.t)).r;

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    tempNormal = normalize(fNormal); // require this for the tangent space conversion.
    tangent = normalize(fTangent);
    biTangent = normalize(fBiTangent);

    viewVec = normalize(-fPosition.xyz);

    // tempNormal is from the object (so in world space, but inaccurate), and normal is from the texture
    normal = texture(normalMap, vec2(fTexCoord.s, fTexCoord.t)).rgb;

    // need to convert the normal to the world space.
    vec3 worldNormal = normal.x * tangent + normal.y * biTangent + normal.z * tempNormal;

    vec3 reflectionVec = reflect(-viewVec, worldNormal);


    vec3 F = FresnelSchlick(max(dot(worldNormal, viewVec), 0.0), F0); // both in world space!
    kS = F;
    kD = 1.0 - kS;
    kD *= (1.0 - metallic);
    vec3 irradiance = texture(irradianceMap, worldNormal).rgb; // need this in world space, so using worldNormal!
    vec3 diffuse = irradiance * albedo;
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, reflectionVec,  roughness * MAX_REFLECTION_LOD).rgb;

    vec2 brdf  = texture(brdfLUT, vec2(max(dot(worldNormal, viewVec), 0.0), roughness)).rg;
    vec3 specularIBL = prefilteredColor * (F * brdf.x + brdf.y);

    fColor = vec4((kD * diffuse + specularIBL) * ao, 1.0);
    
}
