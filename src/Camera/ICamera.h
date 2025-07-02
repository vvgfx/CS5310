#ifndef _ICAMERA_H_
#define _ICAMERA_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

namespace camera
{

    /**
     * This class is an interface for all camera implementations.
     */
    class ICamera
    {
        public:
        /**
         * Return a glm::Lookat style mat4
         */
        virtual glm::mat4 GetViewMatrix()=0;
        /**
         * Takes input a direction and deltaTime to process keyboard input.
         */
        virtual void ProcessKeyboard(Camera_Movement direction, float deltaTime)=0;

        /**
         * Takes an xOffset and yOffset for processing mouse movement.
         */
        virtual void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch=true)=0;

        /**
         * Takes a scroll offset to process zoom.
         */
        virtual void ProcessMouseScroll(float yOffset)=0;

    };
}

#endif