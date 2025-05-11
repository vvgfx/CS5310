#ifndef _LIGHTRETRIEVER_H_
#define _LIGHTRETRIEVER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "DynamicTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>
using namespace std;

namespace sgraph
{
    /**
     * This visitor implements drawing the scene graph using OpenGL
     *
     */
    class LightRetriever : public SGNodeVisitor
    {
    public:


        struct LightProperties
        {
            glm::vec3 ambient;
            glm::vec3 diffuse;
            glm::vec3 specular;
            glm::vec4 position;
        };
        
        /**
         * @brief Construct a new LightRetriever object
         *
         * @param mv a reference to modelview stack that will be used to convert light to the view co-ordinate system
         */
        LightRetriever(stack<glm::mat4> &mv) : modelview(mv)  {}



        void getLights(SGNode *node)
        {

        }

        /**
         * @brief Recur to the children for drawing
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the
         * modelview and color to the shader
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {

        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size() > 0)
            {
                transformNode->getChildren()[0]->accept(this);
            }
            modelview.pop();
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         *
         * @param scaleNode
         */
        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         *
         * @param translateNode
         */
        void visitTranslateTransform(TranslateTransform *translateNode)
        {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode)
        {
            visitTransformNode(rotateNode);
        }

        void visitDynamicTransform(DynamicTransform *dynamicTransformNode)
        {
            visitTransformNode(dynamicTransformNode);
        }

    private:
        stack<glm::mat4> &modelview;
        map<string, glm::mat4> lightMap;
        map<string, 
        vector<glm::mat4> lights;
    };
}

#endif