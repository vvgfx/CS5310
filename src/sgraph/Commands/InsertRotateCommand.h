#ifndef _INSERTROTATECOMMAND_H_
#define _INSERTROTATECOMMAND_H_

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
    class InsertRotateCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new InsertRotateCommand object
         *
         * @param name Name of the node this command should effect
         * @param newNodeName Name of the new node.
         * @param rx rotation in x-axis
         * @param ry rotation in y-axis
         * @param rz rotation in z-axis
         */
        InsertRotateCommand(string name, string newNodeName, float rx, float ry, float rz, float rAngleRadians, sgraph::IScenegraph* scenegraph)
        {
            this->nodeName = name;
            this->newNodeName = newNodeName;
            this->rx = rx;
            this->ry = ry;
            this->rz = rz;
            sgraph = scenegraph;
            this->rAngleRadians = rAngleRadians;
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
            
            sgraph::RotateTransform* rotateNode = new sgraph::RotateTransform(rAngleRadians, rx, ry, rz, newNodeName, sgraph);
            parentNode->addChild(rotateNode);
        }

    private:
        float rx, ry, rz, rAngleRadians;
        sgraph::IScenegraph* sgraph;
        string newNodeName;
    };
}

#endif