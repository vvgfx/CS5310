#ifndef _UPDATELIGHTCOMMAND_H_
#define _UPDATELIGHTCOMMAND_H_

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
    class UpdateLightCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new UpdateLightCommand object
         *
         * @param name Name of the node this command should effect
         * 
         */
        UpdateLightCommand(string name, string lightName, float colorVal[3], float spotDirVal[3], float posVal[3], float angleVal)
        {
            this->nodeName = name;
            this->lightName = lightName;
            copy(colorVal, colorVal + 3, this->color);
            copy(spotDirVal, spotDirVal + 3, this->spotDirection);
            copy(posVal, posVal + 3, this->position);
            this->angle = angleVal;
        }


        /**
         * @brief Override for updating
         *
         * @param groupNode
         */
        void visitGroupNode(sgraph::GroupNode *groupNode)
        {
            updateLightInNode(groupNode);
        }

        /**
         * @brief Override for updating
         *
         *
         * @param leafNode
         */
        void visitLeafNode(sgraph::LeafNode *leafNode)
        {
            updateLightInNode(leafNode);
        }

        /**
         * @brief Override for updating
         *
         * @param parentNode
         */
        void visitParentNode(sgraph::ParentSGNode *parentNode)
        {
            updateLightInNode(parentNode);
        }

        /**
         * @brief Override for updating
         *
         * @param transformNode
         */
        void visitTransformNode(sgraph::TransformNode *transformNode)
        {
            updateLightInNode(transformNode);
        }

        /**
         * @brief Override for updating
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
            updateLightInNode(scaleNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
            updateLightInNode(translateNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
            updateLightInNode(rotateNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitDynamicTransform(sgraph::DynamicTransform *dynamicTransformNode)
        {
            updateLightInNode(dynamicTransformNode);
        }


        void updateLightInNode(sgraph::SGNode* node)
        {   
            vector<util::Light>* lights = node->getLights();

            // auto foundLightIterator = std::find(lights->begin(), lights->end(), dummyLight);
            // if(foundLightIterator != lights->end())
            // {
            //     util::Light* foundLight = &(*foundLightIterator); // dereference from iterator to value, then from value to pointer.
            //     foundLight->setColor(color);
            //     foundLight->setSpotDirection(spotDirection.x, spotDirection.y, spotDirection.z);
            //     foundLight->setSpotAngle(angle);
            //     foundLight->setPosition(position.x, position.y, position.z);
            // }

            for(std::vector<util::Light>::iterator lightIterator = lights->begin(); lightIterator!= lights->end(); lightIterator++)
            {
                if(lightIterator->getName() == lightName)
                {
                    lightIterator->setColor(color[0], color[1], color[2]);
                    lightIterator->setSpotDirection(spotDirection[0], spotDirection[1], spotDirection[2]);
                    lightIterator->setSpotAngle(angle);
                    lightIterator->setPosition(position[0], position[1], position[2]);
                }
            }

        }




    private:
        float color[3];
        float spotDirection[3];
        float position[3];
        float angle;
        string lightName;
    };
}

#endif