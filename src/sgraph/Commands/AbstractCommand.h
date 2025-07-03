#ifndef _ABSTRACTCOMMAND_H_
#define _ABSTRACTCOMMAND_H_

#include "ICommand.h"
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
     * This class exists to provide dummy implementations of SGNodeVisitor, and are expected to be overridden in
     * all of it's children.
     * If any of the SGNodeVisitor overrides are called, an implementation is missing or
     * the function and node are mismatched.
     *
     */
    class AbstractCommand : public ICommand
    {
    public:
        /**
         * @brief Construct a new AbstractCommand object
         *
         */
        AbstractCommand()
        {
        }

        /**
         * @brief Visit Group Node on AbstractCommand
         *
         * @param groupNode
         */
        void visitGroupNode(sgraph::GroupNode *groupNode)
        {
        }

        /**
         * @brief Visit Leaf Node on AbstractCommand
         *
         *
         * @param leafNode
         */
        void visitLeafNode(sgraph::LeafNode *leafNode)
        {
        }

        /**
         * @brief Visit Parent Node on AbstractCommand
         *
         * @param parentNode
         */
        void visitParentNode(sgraph::ParentSGNode *parentNode)
        {
        }

        /**
         * @brief Visit Transform Node on AbstractCommand
         *
         * @param transformNode
         */
        void visitTransformNode(sgraph::TransformNode *transformNode)
        {
        }

        /**
         * @brief Visit Scale Node on AbstractCommand
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
        }

        /**
         * @brief Visit Translate Node on AbstractCommand
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
        }

        /**
         * @brief Visit Rotate Node on AbstractCommand
         *
         * @param translateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
        }

        /**
         * @brief Visit Dynamic Node on AbstractCommand
         *
         * @param translateNode
         */
        void visitDynamicTransform(sgraph::DynamicTransform *dynamicTransformNode)
        {
        }

        /**
         * @brief Visit SRT Node on AbstractCommand
         *
         * @param srtNode
         */
        void visitSRTNode(sgraph::SRTNode* srtNode)
        {
        }

        /**
         * @brief get the name of the node this command is supposed to execute on.
         */
        string getNodeName()
        {
            return this->nodeName;
        }

        /**
         * @brief set the name of the node this command is supposed to execute on.
         * 
         * @param name
         */
        void setNodeName(string name)
        {
            this->nodeName = name;
        }


    private:
    };
}

#endif