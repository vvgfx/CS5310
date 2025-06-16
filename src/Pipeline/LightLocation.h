#ifndef _LIGHTLOCATION_H_
#define _LIGHTLOCATION_H_

namespace pipeline
{
    /**
     * This class provides a mapping of each light property to it's position in the Shader program.
     */
    class LightLocation 
    {

    public:
        int ambient,diffuse,specular,position, color;
        int spotDirection, spotAngle;
        LightLocation()
        {
            ambient = diffuse = specular = position = color = -1;
            spotDirection = spotAngle = -1;
        }

    };
}

#endif