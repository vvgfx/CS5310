#ifndef _INSERTSRTCOMMAND_H_
#define _INSERTSRTCOMMAND_H_

#include "ICommand.h"
#include "AbstractCommand.h"
#include "../GroupNode.h"
#include "../LeafNode.h"
#include "../TransformNode.h"
#include "../RotateTransform.h"
#include "../ScaleTransform.h"
#include "../TranslateTransform.h"
#include "../DynamicTransform.h"
#include "../SRTNode.h"
#include <stack>
#include <iostream>
using namespace std;

namespace command
{
    /**
     * This command uses the command design pattern to make updates to a scenegraph's nodes
     * This class inserts a rotate node under the provided parent.
     *
     */
    class InsertSRTCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new InsertSRTCommand object
         *
         * @param name Name of the node this command should effect
         * @param newNodeName Name of the new node.
         * @param sx scale in x-axis
         * @param sy scale in y-axis
         * @param sz scale in z-axis
         * @param rx rotation in x-axis (radians)
         * @param ry rotation in y-axis (radians)
         * @param rz rotation in z-axis (radians)
         * @param tx translation in x-axis
         * @param ty translation in y-axis
         * @param tz translation in z-axis
         */
        InsertSRTCommand(string name, string newNodeName, float sx, float sy, float sz,float rx, float ry, float rz, float tx, float ty, float tz, sgraph::IScenegraph* scenegraph)
        {
            this->nodeName = name;
            this->newNodeName = newNodeName;

            // converting to radians here.
            this->rx = rx;
            this->ry = ry;
            this->rz = rz;

            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
            this->sx = sx;
            this->sy = sy;
            this->sz = sz;
            sgraph = scenegraph;
        }

        /**
         * @brief Visit Group Node on InsertTranslateCommand
         *
         * @param groupNode
         */
        void visitGroupNode(sgraph::GroupNode *groupNode)
        {
            addChildren(groupNode);
        }

        /**
         * @brief Visit Scale Node on InsertTranslateCommand
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
            if(scaleNode->getChildren().size() > 0)
                return;
            addChildren(scaleNode);
        }

        /**
         * @brief Visit Translate Node on InsertTranslateCommand
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
            if(translateNode->getChildren().size() > 0)
                return;
            addChildren(translateNode);
        }

        /**
         * @brief Visit Rotate Node on InsertTranslateCommand
         *
         * @param translateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
            if(rotateNode->getChildren().size() > 0)
                return;
            addChildren(rotateNode);
        }

        /**
         * @brief Visit Dynamic Node on InsertTranslateCommand
         *
         * @param translateNode
         */
        void visitDynamicTransform(sgraph::DynamicTransform *dynamicTransformNode)
        {
            if(dynamicTransformNode->getChildren().size() > 0)
                return;
            addChildren(dynamicTransformNode);
        }

        void visitSRTNode(sgraph::SRTNode* srtNode)
        {
            if(srtNode->getChildren().size() > 0)
                return;
            addChildren(srtNode);
        }


        /**
         * These nodes can have only 1 child.
         */
        void addChildren(sgraph::ParentSGNode* parentNode)
        {
            
            sgraph::SRTNode* srtNode = new sgraph::SRTNode(tx, ty, tz, rx, ry, rz, sx, sy, sz, newNodeName, sgraph);
            parentNode->addChild(srtNode);
        }

    private:
        float rx, ry, rz;
        float sx, sy, sz;
        float tx, ty, tz;
        sgraph::IScenegraph* sgraph;
        string newNodeName;
    };
}

#endif