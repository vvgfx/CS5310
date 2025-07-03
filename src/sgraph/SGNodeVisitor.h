#ifndef _SGNODEVISITOR_H_
#define _SGNODEVISITOR_H_

#include <string>
using namespace std;

/*
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "ScaleTransform.h"
#include "RotateTransform.h"
#include "TranslateTransform.h"
*/

namespace sgraph {
    class GroupNode;
    class LeafNode;
    class TransformNode;
    class ScaleTransform;
    class RotateTransform;
    class TranslateTransform;
    class DynamicTransform;
    class SRTNode;

/**
 * This class represents the interface for a visitor on scene graph nodes.
 * The visitor design pattern ensures that one can implement various 
 * functions on the scene graph nodes, while not having to change the nodes.
 * 
 * 
 */
    class SGNodeVisitor {
        public:
        virtual void visitGroupNode(GroupNode *node)=0;
        virtual void visitLeafNode(LeafNode *node)=0;
        virtual void visitTransformNode(TransformNode *node)=0;
        virtual void visitScaleTransform(ScaleTransform *node)=0;
        virtual void visitTranslateTransform(TranslateTransform *node)=0;
        virtual void visitRotateTransform(RotateTransform *node)=0;
        virtual void visitDynamicTransform(DynamicTransform *node)=0;
        virtual void visitSRTNode(SRTNode* node)=0;
    };
}

#endif