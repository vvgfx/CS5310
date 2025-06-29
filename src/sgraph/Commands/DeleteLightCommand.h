#ifndef _DELETELIGHTCOMMAND_H_
#define _DELETELIGHTCOMMAND_H_

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
#include <algorithm>
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
    class DeleteLightCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new DeleteLightCommand object
         *
         * @param name Name of the node this command should effect
         * 
         */
        DeleteLightCommand(string name, string lightName)
        {
            this->nodeName = name;
            this->lightName = lightName;
        }


        /**
         * @brief Override for updating
         *
         * @param groupNode
         */
        void visitGroupNode(sgraph::GroupNode *groupNode)
        {
            deleteLightInNode(groupNode);
        }

        /**
         * @brief Override for updating
         *
         *
         * @param leafNode
         */
        void visitLeafNode(sgraph::LeafNode *leafNode)
        {
            deleteLightInNode(leafNode);
        }

        /**
         * @brief Override for updating
         *
         * @param parentNode
         */
        void visitParentNode(sgraph::ParentSGNode *parentNode)
        {
            deleteLightInNode(parentNode);
        }

        /**
         * @brief Override for updating
         *
         * @param transformNode
         */
        void visitTransformNode(sgraph::TransformNode *transformNode)
        {
            deleteLightInNode(transformNode);
        }

        /**
         * @brief Override for updating
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
            deleteLightInNode(scaleNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
            deleteLightInNode(translateNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
            deleteLightInNode(rotateNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitDynamicTransform(sgraph::DynamicTransform *dynamicTransformNode)
        {
            deleteLightInNode(dynamicTransformNode);
        }


        void deleteLightInNode(sgraph::SGNode* node)
        {   
            vector<util::Light>* lights = node->getLights();

            for(std::vector<util::Light>::iterator lightIterator = lights->begin(); lightIterator!= lights->end(); lightIterator++)
            {
                if(lightIterator->getName() == lightName)
                {
                    lights->erase(lightIterator);
                    break;
                }
            }

        }




    private:
        string lightName;
    };
}

#endif