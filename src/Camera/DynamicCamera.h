#ifndef _DYNAMICCAMERA_H_
#define _DYNAMICCAMERA_H_

#include "ICamera.h"
#include "../sgraph/DynamicTransform.h"

/**
 * This camera requires a node of type camera in the scenegraph. This node must be attached to the root, and cannot be modified.
 */


// Default camera values
const float MOVESPEED       =  300.0f;
const float MOUSESENSITIVITY =  0.1f;

namespace camera
{

    // An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
    class DynamicCamera : public ICamera
    {
    public:

        glm::vec3 cameraEye;
        glm::vec3 cameraLookAt;
        glm::vec3 cameraUp;
        glm::vec3 cameraPosition;

        sgraph::DynamicTransform* cameraNode;
    
        // constructor with vectors
        DynamicCamera(glm::vec3 position, sgraph::DynamicTransform* camNode) : cameraPosition(position), cameraNode(camNode)
        {
        }
    
        // returns the view matrix calculated using Euler Angles and the LookAt Matrix
        glm::mat4 GetViewMatrix()
        {
            glm::mat4 viewMat(1.0f);
            glm::mat4 cameraTransformMatrix = glm::translate(glm::mat4(1.0f), cameraPosition) * cameraNode->getTransformMatrix();
            cameraEye = cameraTransformMatrix * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // setting 1 as the homogenous coordinate
            cameraLookAt = cameraTransformMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 1.0f);
            cameraUp = cameraTransformMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);//homogenous coordinate is 0.0f as the vector is an axis, not a point.
            
            viewMat = viewMat * glm::lookAt(cameraEye, cameraLookAt, cameraUp);        
            return viewMat;
        }
    
        // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
        void ProcessKeyboard(Camera_Movement direction, float deltaTime = 0.167)
        {
            float forwardDirSpeed = direction == FORWARD ? MOVESPEED : direction == BACKWARD ? -MOVESPEED : 0.0;
            float horizontalDirSpeed = direction == RIGHT ? MOVESPEED : direction == LEFT ? -MOVESPEED : 0.0;
            forwardDirSpeed *= deltaTime;
            horizontalDirSpeed *= deltaTime;
            glm::mat4 translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(horizontalDirSpeed, 0.0f, -forwardDirSpeed)); // negative forward direction because the camera is pointed towards the negative x-axis. So moving forward should actually decrement the z-position
            cameraNode->postmulTransformMatrix(translateMatrix);

        }
    
        // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
        {
            glm::mat4 rotateMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-xoffset * MOUSESENSITIVITY), glm::vec3(0.0f, 1.0f, 0.0f));
            rotateMatrix = glm::rotate(rotateMatrix, glm::radians(yoffset * MOUSESENSITIVITY), glm::vec3(1.0f, 0.0f, 0.0f)); // screen space is 0,0 at the top left, so need to invert the pitchDir
            cameraNode->postmulTransformMatrix(rotateMatrix);
        }
    
        // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
        void ProcessMouseScroll(float yoffset)
        {
            // not supported for now.
        }
    
    private:
    };
}
#endif