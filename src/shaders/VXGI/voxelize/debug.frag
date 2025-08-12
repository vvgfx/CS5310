#version 460 core

in vec4 worldPos;
out vec4 fColor;

uniform sampler3D voxelTexture;
uniform vec4 gridMin;
uniform vec4 gridMax;

void main()
{
    // convert world to uvw - to use in voxelTexture sampling.
    vec3 uvw = (worldPos.xyz - gridMin.xyz) / ( gridMax.xyz - gridMin.xyz);

    // should not be required, the scene is less than the max voxel size.
    if (any(lessThan(uvw, vec3(0))) || any(greaterThan(uvw, vec3(1)))) {
        fColor = vec4(1, 0, 0, 1); 
        return;
    }

    // fColor = vec4(0,1,0,1);
    // return;

    vec4 voxel = texture(voxelTexture, uvw);

    fColor = vec4(voxel.rgb * 5.0, 1.0);

    // if (voxel.a > 0.001) 
    //     fColor = mix(fColor, vec4(0, 1, 0, 1), 0.3);  // Tint green
    //     // fColor = vec4(0,1,1,1);
}
