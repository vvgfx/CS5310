#ifndef _DYNAMICTRANSFORM_H_
#define _DYNAMICTRANSFORM_H_

#include "SGNode.h"
#include "TransformNode.h"
#include "IScenegraph.h"
#include <glm/gtc/matrix_transform.hpp>


/**
 * This is used when you need to move an object at runtime. You can pass the transform matrix and it will update.
 */

namespace sgraph 
{

    class DynamicTransform: public TransformNode
    {
        glm::mat4 transformMatrix;

        ParentSGNode *copyNode()
        {
            return new DynamicTransform(this->transformMatrix, name, scenegraph);
        }

        public:
            DynamicTransform(glm::mat4 transformMatrix, const string& name,sgraph::IScenegraph *graph):TransformNode(name, graph)
            {
                this->transformMatrix = transformMatrix;
                setTransform(this->transformMatrix);
            }

            /**
             * Visitor pattern code for this.
             */
            void accept(SGNodeVisitor* visitor) 
            {
                return visitor->visitDynamicTransform(this);
            }

            glm::mat4 getTransformMatrix()
            {
                return transformMatrix;
            }

            /**
             * When you want to update the transform wrt the parent's co-ordinate system, pass the matrix that you need to premultiply.
             */
            void premulTransformMatrix(glm::mat4 matToPremultiply)
            {
                this->transformMatrix = matToPremultiply * this->transformMatrix;
                setTransform(this->transformMatrix);
            }
            
            /**
             * When you want to update the transform wrt the local co-ordinate system, pass the matrix you need to post-multiply. This might be useful for composite transformations.
             */
            
            void postmulTransformMatrix(glm::mat4 matToPostMultilply)
            {
                this->transformMatrix = this->transformMatrix * matToPostMultilply;
                setTransform(this->transformMatrix);
            }

            /**
             * Use this when you need to reset/ set the transform matrix to something new altogether.
             */
            void setTransformMatrix(glm::mat4 newMat)
            {
                this->transformMatrix = newMat;
                setTransform(this->transformMatrix);
            }
    };
}


#endif