#ifndef _INSERTLEAFCOMMAND_H_
#define _INSERTLEAFCOMMAND_H_

#include "ICommand.h"
#include "AbstractCommand.h"
#include "../GroupNode.h"
#include "../LeafNode.h"
#include "../TransformNode.h"
#include "../RotateTransform.h"
#include "../ScaleTransform.h"
#include "../TranslateTransform.h"
#include "../DynamicTransform.h"
#include <stack>
#include <iostream>
using namespace std;

namespace command
{
    /**
     * This command uses the command design pattern to make updates to a scenegraph's nodes
     * This class inserts a translate node under the provided child.
     *
     */
    class InsertLeafCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new InsertLeafCommand object
         *
         * @param name Name of the node this command should effect
         * @param newNodeName Name of the new node
         * @param scenegraph The scenegraph that this node should be attached to
         * @param material Util::Material of this leaf
         * @param texName name of the texture. Pass empty string if not used.
         */
        InsertLeafCommand(string name, string newNodeName, sgraph::IScenegraph *scenegraph, util::Material mat, string instanceOf, string texName)
        {
            this->material = mat;
            this->nodeName = name;
            this->newNodeName = newNodeName;
            sgraph = scenegraph;
            this->instanceOf = instanceOf;
            this->textureName = texName;
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
            if (scaleNode->getChildren().size() > 0)
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
            if (translateNode->getChildren().size() > 0)
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
            if (rotateNode->getChildren().size() > 0)
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
            if (dynamicTransformNode->getChildren().size() > 0)
                return;
            addChildren(dynamicTransformNode);
        }

        /**
         * These nodes can have only 1 child.
         */
        void addChildren(sgraph::ParentSGNode *parentNode)
        {
            sgraph::LeafNode *leafNode = new sgraph::LeafNode(instanceOf, newNodeName, sgraph, textureName);
            leafNode->setMaterial(material);
            parentNode->addChild(leafNode);
        }

    private:
        sgraph::IScenegraph *sgraph;
        string newNodeName;
        util::Material material;
        string instanceOf, textureName;
    };
}

#endif