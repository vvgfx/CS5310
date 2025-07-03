#ifndef _NORMALGLSCENEGRAPHRENDERER_H_
#define _NORMALGLSCENEGRAPHRENDERER_H_

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
     * This visitor implements drawing the scene graph using OpenGL
     * 
     */
    class NormalGLScenegraphRenderer: public SGNodeVisitor {
        public:
        /**
         * @brief Construct a new NormalGLScenegraphRenderer object
         * 
         * @param mv a reference to modelview stack that will be used while rendering
         * @param os the map of ObjectInstance objects
         * @param shaderLocations the shader locations for the program used to render
         * @param texIdMap the map of textures to their locations in GPU memory
         */
        NormalGLScenegraphRenderer(stack<glm::mat4>& mv,map<string,util::ObjectInstance *>& os,util::ShaderLocationsVault& shaderLocations, map<string, unsigned int>& texIdMap) 
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
            //send modelview matrix to GPU  
            // glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(leafNode->getMaterial().getAmbient())); // Commenting this out beacuse I no longer have vColor
            // adding lighting stuff here

            //vertex first
            // glm::mat4 normalmatrix = glm::inverse(glm::transpose((modelview.top()))); Moved this to the vertex shader instead of calculating on the cpu
            glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview.top()));
            // glUniformMatrix4fv(shaderLocations.getLocation("normalmatrix"), 1, GL_FALSE, glm::value_ptr(normalmatrix));
            
            //fragment next
            util::Material leafMat = leafNode->getMaterial();
            glUniform3fv(shaderLocations.getLocation("material.ambient"), 1, glm::value_ptr(leafMat.getAmbient()));
            glUniform3fv(shaderLocations.getLocation("material.diffuse"), 1, glm::value_ptr(leafMat.getDiffuse()));
            glUniform3fv(shaderLocations.getLocation("material.specular"), 1, glm::value_ptr(leafMat.getSpecular()));
            glUniform1f(shaderLocations.getLocation("material.shininess"), leafMat.getShininess());

            //texture stuff here!
            glUniformMatrix4fv(shaderLocations.getLocation("texturematrix"), 1, GL_FALSE, glm::value_ptr(leafNode->getTextureTransform()));
            //TODO: need to move the texture to GPU memory in the scenegraphImporter, and then pass it to NormalGLScenegraphRenderer's constructor. - done    
            string texName = leafNode->getTextureMap();
            if (!texName.empty() && textureIdMap.find(texName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[texName];
                // glEnable(GL_TEXTURE_2D); // commenting this because it throws error 1280 and is not required anyways.
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("image"), 0);
            }
            
            //similarly, do the same for normal textures!
            // setting normal textures as texture 1
            string normalTexName = leafNode->getNormalMap();
            bool isBumpMapping = leafNode->getPBRBool();
            glUniform1i(shaderLocations.getLocation("bumpMapping"), isBumpMapping);
            // cout<<"bump mapping: "<<isBumpMapping<<endl;
            //passing whether bump mapping is used for this object. This is because the normal map I built doesn't seem to work.
            if (isBumpMapping && !normalTexName.empty() && textureIdMap.find(normalTexName) != textureIdMap.end()) {
                unsigned int texID = textureIdMap[normalTexName];
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, texID);
                glUniform1i(shaderLocations.getLocation("normalImage"), 1);
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

        void visitSRTNode(SRTNode* srtNode)
        {
            visitTransformNode(srtNode);
        }
        

        private:
        stack<glm::mat4>& modelview;    
        util::ShaderLocationsVault shaderLocations;
        map<string,util::ObjectInstance *> objects;
        map<string, unsigned int>& textureIdMap;

   };
}

#endif