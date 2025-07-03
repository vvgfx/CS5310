#ifndef _DELETENODECOMMAND_h_
#define _DELETENODECOMMAND_h_

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
     * This class deletes a node from the provided parent.
     *
     */
    class DeleteNodeCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new DeleteNodeCommand object
         *
         * @param name Name of the node this command should effect (in this case, this is the parent)
         * @param childName name of the child you want to delete.
         */
        DeleteNodeCommand(string name, string childName, sgraph::IScenegraph* scenegraph)
        {
            this->nodeName = name;
            this->childName = childName;
            sgraph = scenegraph;
        }

        /**
         * @brief Visit Group Node on InsertTranslateCommand
         *
         * @param groupNode
         */
        void visitGroupNode(sgraph::GroupNode *groupNode)
        {
            cout<<"Adding group node in command"<<endl;
            deleteChild(groupNode);
        }

        /**
         * @brief Visit Scale Node on InsertTranslateCommand
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
            if(scaleNode->getChildren().size() < 1)
                return;
            deleteChild(scaleNode);
        }

        /**
         * @brief Visit Translate Node on InsertTranslateCommand
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
            if(translateNode->getChildren().size() < 1)
                return;
            deleteChild(translateNode);
        }

        /**
         * @brief Visit Rotate Node on InsertTranslateCommand
         *
         * @param translateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
            if(rotateNode->getChildren().size() < 1)
                return;
            deleteChild(rotateNode);
        }

        /**
         * @brief Visit Dynamic Node on InsertTranslateCommand
         *
         * @param translateNode
         */
        void visitDynamicTransform(sgraph::DynamicTransform *dynamicTransformNode)
        {
            if(dynamicTransformNode->getChildren().size() < 1)
                return;
            deleteChild(dynamicTransformNode);
        }

        void visitSRTNode(sgraph::SRTNode* srtNode)
        {
            if(srtNode->getChildren().size() < 1)
                return;
            deleteChild(srtNode);
        }


        /**
         * delete node from parent.
         */
        void deleteChild(sgraph::ParentSGNode* parentNode)
        {
            cout<<"deleting child"<<endl;
            parentNode->removeChild(childName);
        }

    private:
        sgraph::IScenegraph* sgraph;
        string childName;
    };
}

#endif