#ifndef _OLDPBRRENDERER_H_
#define _OLDPBRRENDERER_H_

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

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL.
     * Note that this Renderer requires all the materials required for a PBR workflow (albedoMap, normalMap, roughnessMap, metalicMap, ambientOcclusionMap)
     * 
     */
    class OldPBRRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new OldPBRRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         */
        OldPBRRenderer(stack<glm::mat4>& mv,map<string,util::ObjectInstance *>& os,util::ShaderLocationsVault& shaderLocations, map<string, unsigned int>& texIdMap) 
            : modelview(mv)
            , objects(os)
            , textureIdMap(texIdMap) {
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
            // glm::mat4 normalmatrix = glm::inverse(glm::transpose((modelview.top()))); Moved this to the vertex shader instead of calculating on the cpu
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            // glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, GL_FALSE, glm::value_ptr(normalmatrix));
            
            //fragment next
            util::Material leafMat = leafNode->getMaterial();
            // Don't need any of this for the pbr workflow.
            // glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(leafMat.getAmbient()));
            // glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(leafMat.getDiffuse()));
            // glUniform3fv(shaderLocations.getLocation("material.specular"), 1, glm::value_ptr(leafMat.getSpecular()));
            // glUniform1f(shaderLocations.getLocation("material.shininess"), leafMat.getShininess());

            //texture stuff here!

            // albedo first
            glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1, GL_FALSE, glm::value_ptr(leafNode->getTextureTransform()));
            string albedoMapName = leafNode->getTextureMap();
            if (!albedoMapName.empty() && textureIdMap.find(albedoMapName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[albedoMapName];
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("albedoMap"), 0);
            }
            
            //similarly, do the same for normal textures!
            //setting normal textures as texture 1
            string normalMapName = leafNode->getNormalMap();
            bool isPBR = leafNode->getPBRBool();
            glUniform1i(shaderLocations.getLocation("PBR"), isPBR);
            if (isPBR && !normalMapName.empty() && textureIdMap.find(normalMapName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[normalMapName];
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("normalMap"), 1);
            }

            //roughness is texture 2
            string roughnessMapName = leafNode->getRoughnessMap();
            if (isPBR && !roughnessMapName.empty() && textureIdMap.find(roughnessMapName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[roughnessMapName];
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("roughnessMap"), 2);
            }

            //metallic is texture 3
            string metallicMapName = leafNode->getMetallicMap();
            if (isPBR && !metallicMapName.empty() && textureIdMap.find(metallicMapName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[metallicMapName];
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("metallicMap"), 3);
            }
            
            //ambient occlusion is texture 4
            string aoMapName = leafNode->getAOMap();
            if (isPBR && !aoMapName.empty() && textureIdMap.find(aoMapName) != textureIdMap.end()) {
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

        private:
        stack<glm::mat4>& modelview;    
        util::ShaderLocationsVault shaderLocations;
        map<string,util::ObjectInstance *> objects;
        map<string, unsigned int>& textureIdMap;

   };
}

#endif