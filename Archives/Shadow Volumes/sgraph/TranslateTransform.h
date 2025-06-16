#ifndef _TRANSLATETRANSFORM_H_
#define _TRANSLATETRANSFORM_H_


#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace sgraph {
    /**
     * This node represents a specific kind of transform: translation
     * 
     */
    class TranslateTransform: public TransformNode {
        protected:
            float tx,ty,tz;

        ParentSGNode *copyNode() {
            return new TranslateTransform(tx,ty,tz,name,scenegraph);
        }

        public:
            TranslateTransform(float tx,float ty,float tz,const string& name,sgraph::IScenegraph *graph) 
                :TransformNode(name,graph) {
                    this->tx = tx;
                    this->ty = ty;
                    this->tz = tz;
                    glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz));
                    setTransform(transform);
            }

            
            /**
             * Visit this node.
             * 
             */
            void accept(SGNodeVisitor* visitor) {
                visitor->visitTranslateTransform(this);
            }

            glm::vec3 getTranslate() {
            return glm::vec3(tx,ty,tz);
            }
            
            void updateTransform(float x, float y, float z)
            {
                this->tx += x;
                this->ty += y;
                this->tz += z;
                cout<<"New translate: "<<tx<<" , "<<ty<<" , "<<tz<<endl;
                glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz));
                setTransform(transform);
            }

    };
}

#endif