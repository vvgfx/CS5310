#ifndef _VOXEL_RENDERER_H_
#define _VOXEL_RENDERER_H_

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

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL.
     * This renderer is used to provide data to voxelize the scene. It requires textured PBR workflow.
     */
    class VoxelRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new VoxelRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        VoxelRenderer(stack<glm::mat4>& mv,map<string,util::ObjectInstance *>& os,util::ShaderLocationsVault& shaderLocations, map<string, unsigned int>& texMap) 
            : modelview(mv)
            , objects(os)
            , textureIdMap(texMap) {
            this->shaderLocations = shaderLocations;
            for (map<string,util::ObjectInstance *>::iterator it=objects.begin();it!=objects.end();it++) {
                cout << "Mesh with name: "<< it->first << endl;
            }
        }

        /**
         * @brief Recur to the children for drawing
         * 
         * @param groupNode 
         */
        void visitGroupNode(GroupNode *groupNode) {
            for (int i=0;i<groupNode->getChildren().size();i=i+1) {
                groupNode->getChildren()[i]->accept(this);
            }
        }

        /**
         * @brief Draw the instance for the leaf, after passing the 
         * modelview and color to the shader
         * 
         * @param leafNode 
         */
        void visitLeafNode(LeafNode *leafNode) {
            //vertex first
            glm::mat4 normalmatrix = glm::inverse(glm::transpose((modelview.top())));
            glUniformMatrix4fv(shaderLocations.getLocation("model"), 1, GL_FALSE, glm::value_ptr(modelview.top())); // This contains only the model-to-world transformation
            glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, GL_FALSE, glm::value_ptr(normalmatrix));
             glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1, GL_FALSE, glm::value_ptr(leafNode->getTextureTransform()));


            string albedoMapName = leafNode->getTextureMap();
            if (!albedoMapName.empty() && textureIdMap.find(albedoMapName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[albedoMapName];
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("albedoMap"), 0);
            }

            // //roughness is texture 2
            // string roughnessMapName = leafNode->getRoughnessMap();
            // if (!roughnessMapName.empty() && textureIdMap.find(roughnessMapName) != textureIdMap.end()) {
            //     unsigned int texID = textureIdMap[roughnessMapName];
            //     glActiveTexture(GL_TEXTURE2);
            //     glBindTexture(GL_TEXTURE_2D, texID);
            //     glUniform1i(shaderLocations.getLocation("roughnessMap"), 2);
            // }

            // // //metallic is texture 3
            // string metallicMapName = leafNode->getMetallicMap();
            // if (!metallicMapName.empty() && textureIdMap.find(metallicMapName) != textureIdMap.end()) {
            //     unsigned int texID = textureIdMap[metallicMapName];
            //     glActiveTexture(GL_TEXTURE3);
            //     glBindTexture(GL_TEXTURE_2D, texID);
            //     glUniform1i(shaderLocations.getLocation("metallicMap"), 3);
            // }

            objects[leafNode->getInstanceOf()]->draw();
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         * 
         * @param transformNode 
         */
        void visitTransformNode(TransformNode * transformNode) {
            modelview.push(modelview.top());
            modelview.top() = modelview.top() * transformNode->getTransform();
            if (transformNode->getChildren().size()>0) {
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
        void visitScaleTransform(ScaleTransform *scaleNode) {
            visitTransformNode(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitTransformNode above
         * 
         * @param translateNode 
         */
        void visitTranslateTransform(TranslateTransform *translateNode) {
            visitTransformNode(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode) {
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
        stack<glm::mat4>& modelview;    
        util::ShaderLocationsVault shaderLocations;
        map<string,util::ObjectInstance *>& objects;
        map<string, unsigned int>& textureIdMap;

   };
}

#endif