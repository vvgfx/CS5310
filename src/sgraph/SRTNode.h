#ifndef _SRTNODE_H_
#define _SRTNODE_H_


#include "TransformNode.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace sgraph {
    /**
     * This node represents a specific kind of transform: translation
     * 
     */
    class SRTNode: public TransformNode {
        protected:
            float tx,ty,tz;
            float rx, ry, rz;
            float sx, sy, sz;

        ParentSGNode *copyNode() {
            return new SRTNode(tx,ty,tz,rx, ry, rz, sx, sy, sz, name,scenegraph);
        }

        public:
            // Note that all angles are in radians
            SRTNode(float tx,float ty,float tz,float rx, float ry, float rz, float sx, float sy, float sz, const string& name,sgraph::IScenegraph *graph) 
                :TransformNode(name,graph) {
                    this->tx = tx;
                    this->ty = ty;
                    this->tz = tz;
                    this->rx = rx;
                    this->ry = ry;
                    this->rz = rz;
                    this->sx = sx;
                    this->sy = sy;
                    this->sz = sz;

                    // following unity convention, so the final matrix is : T × Ry × Rx × Rz × S × V
                    glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz)) * glm::rotate(glm::mat4(1.0), ry, glm::vec3(0.0, 1.0, 0.0)) 
                                            * glm::rotate(glm::mat4(1.0), rx, glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0), rz, glm::vec3(0.0, 0.0, 1.0))
                                            * glm::scale(glm::mat4(1.0), glm::vec3(sx, sy, sz));
                    setTransform(transform);
            }

            
            /**
             * Visit this node.
             * 
             */
            void accept(SGNodeVisitor* visitor) {
                visitor->visitSRTNode(this);
            }

            glm::vec3 getTranslate() {
            return glm::vec3(tx,ty,tz);
            }

            glm::vec3 getRotate() {
            return glm::vec3(rx,ry,rz);
            }

            glm::vec3 getScale() {
            return glm::vec3(sx,sy,sz);
            }


            void setAbsoluteTransform(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz)
            {
                this->tx = tx;
                this->ty = ty;
                this->tz = tz;

                this->rx = rx;
                this->ry = ry;
                this->rz = rz;

                this->sx = sx;
                this->sy = sy;
                this->sz = sz;

                glm::mat4 transform = glm::translate(glm::mat4(1.0),glm::vec3(tx,ty,tz)) * glm::rotate(glm::mat4(1.0), ry, glm::vec3(0.0, 1.0, 0.0)) 
                                            * glm::rotate(glm::mat4(1.0), rx, glm::vec3(1.0, 0.0, 0.0)) * glm::rotate(glm::mat4(1.0), rz, glm::vec3(0.0, 0.0, 1.0))
                                            * glm::scale(glm::mat4(1.0), glm::vec3(sx, sy, sz));
                setTransform(transform);
            }

    };
}

#endif