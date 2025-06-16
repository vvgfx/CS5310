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
#include "Light.h"
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
        /**
         * @brief Construct a new LightRetriever object
         *
         * @param mv a reference to modelview stack that will be used to convert light to the view co-ordinate system
         */
        LightRetriever(stack<glm::mat4> &mv) :lightModelview(mv) {
            // lightModelview = mv;
        }

        void saveLights(SGNode *node)
        {
            vector<util::Light> nodeLights =  node->getLights();
            if(nodeLights.empty())
                return;
            for(auto& light : nodeLights)
            {
                lights.push_back(light);
                lightTransformations.push_back(lightModelview.top());
            }
        }

        /**
         * @brief Recur to the children for drawing
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            saveLights(groupNode);
            for (int i = 0; i < groupNode->getChildren().size(); i = i + 1)
            {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief get lights attached to this leaf(if any)
         * 
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {
            saveLights(leafNode);
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            lightModelview.push(lightModelview.top());
            lightModelview.top() = lightModelview.top() * transformNode->getTransform();
            saveLights(transformNode);
            if (transformNode->getChildren().size() > 0)
            {
                transformNode->getChildren()[0]->accept(this);
            }
            lightModelview.pop();
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

        vector<util::Light> getLights()
        {
            return lights;
        }

        vector<glm::mat4> getLightTransformations()
        {
            return lightTransformations;
        }

        void clearData()
        {
            lights.clear();
            lightTransformations.clear(); 
        }

    private:
        stack<glm::mat4>& lightModelview;

        // Each light has ambient, diffuse, specular, shininess and position. Now the first 4 are independent of the co-ordinate system, but the position isn't. 
        // So I'm keeping a track of the lightTransformations such that for each light, there is a lightTransformation. This may duplicate transformations if there
        // are multiple lights in the same node, but so be it! This way I can keep the index constant when I ultimately reference them.
        vector<util::Light> lights;
        vector<glm::mat4> lightTransformations;
    };
}

#endif