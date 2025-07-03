#ifndef _TEXTUREDPBRAMBIENTRENDERER_H_
#define _TEXTUREDPBRAMBIENTRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "DynamicTransform.h"
#include "SRTNode.h"
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
    class TexturedPBRAmbientRenderer : public SGNodeVisitor
    {
    public:
        /**
         * @brief Construct a new TexturedPBRAmbientRenderer object
         *
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         * @param texMap the map of texture names to locations in GPU memory.
         */
        TexturedPBRAmbientRenderer(stack<glm::mat4> &mv, map<string, util::ObjectInstance *> &os, util::ShaderLocationsVault &shaderLocations, map<string, unsigned int> &texMap)
            : modelview(mv), objects(os), textureIdMap(texMap)
        {
            this->shaderLocations = shaderLocations;
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
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            util::Material leafMat = leafNode->getMaterial();
            glm::mat4 normalmatrix = glm::inverse(glm::transpose((modelview.top())));
            glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1, GL_FALSE, glm::value_ptr(leafNode->getTextureTransform()));

            // textures for albedo and ambient occlusion here
            string albedoMapName = leafNode->getTextureMap();
            if (!albedoMapName.empty() && textureIdMap.find(albedoMapName) != textureIdMap.end())
            {
                unsigned int texID = textureIdMap[albedoMapName];
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("albedoMap"), 0);
            }

            string aoMapName = leafNode->getAOMap();
            if (!aoMapName.empty() && textureIdMap.find(aoMapName) != textureIdMap.end())
            {
                unsigned int texID = textureIdMap[aoMapName];
                glActiveTexture(GL_TEXTURE4);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("aoMap"), 4);
            }

            objects[leafNode->getInstanceOf()]->draw();
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

        void visitSRTNode(SRTNode* srtNode)
        {
            visitTransformNode(srtNode);
        }

    private:
        stack<glm::mat4> &modelview;
        util::ShaderLocationsVault shaderLocations;
        map<string, util::ObjectInstance *> objects;
        map<string, unsigned int> &textureIdMap;
    };
}

#endif