#ifndef _INSERTLIGHTCOMMAND_H_
#define _INSERTLIGHTCOMMAND_H_

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
#include <algorithm>
using namespace std;

namespace command
{
    /**
     * This command uses the command design pattern to make updates to a scenegraph's nodes
     *
     */
    class InsertLightCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new InsertLightCommand object
         *
         * @param name Name of the node this command should effect
         * 
         */
        InsertLightCommand(string name, string lightName, glm::vec3 colorVal, glm::vec4 spotDirVal, glm::vec4 posVal, float angleVal)
        {
            this->nodeName = name;
            this->lightName = lightName;
            this->color = colorVal;
            this->spotDirection = spotDirVal;
            this->position = posVal;
            this->angle = angleVal;
        }


        /**
         * @brief Override for updating
         *
         * @param groupNode
         */
        void visitGroupNode(sgraph::GroupNode *groupNode)
        {
            insertLightInNode(groupNode);
        }

        /**
         * @brief Override for updating
         *
         *
         * @param leafNode
         */
        void visitLeafNode(sgraph::LeafNode *leafNode)
        {
            insertLightInNode(leafNode);
        }

        /**
         * @brief Override for updating
         *
         * @param parentNode
         */
        void visitParentNode(sgraph::ParentSGNode *parentNode)
        {
            insertLightInNode(parentNode);
        }

        /**
         * @brief Override for updating
         *
         * @param transformNode
         */
        void visitTransformNode(sgraph::TransformNode *transformNode)
        {
            insertLightInNode(transformNode);
        }

        /**
         * @brief Override for updating
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
            insertLightInNode(scaleNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
            insertLightInNode(translateNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
            insertLightInNode(rotateNode);
        }

        /**
         * @brief Override for updating
         *
         * @param translateNode
         */
        void visitDynamicTransform(sgraph::DynamicTransform *dynamicTransformNode)
        {
            insertLightInNode(dynamicTransformNode);
        }

        void visitSRTNode(sgraph::SRTNode* srtNode)
        {
            insertLightInNode(srtNode);
        }


        void insertLightInNode(sgraph::SGNode* node)
        {   
            util::Light newLight;
            newLight.setColor(color);
            newLight.setSpotDirection(spotDirection.x, spotDirection.y, spotDirection.z);
            newLight.setSpotAngle(angle);
            newLight.setPosition(position);
            
            node->addLight(newLight);
        }




    private:
        glm::vec3 color;
        glm::vec4 spotDirection;
        glm::vec4 position;
        float angle;
        string lightName;
    };
}

#endif