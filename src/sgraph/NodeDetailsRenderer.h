#ifndef _NODEDETAILSRENDERER_H_
#define _NODEDETAILSRENDERER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "DynamicTransform.h"
#include "Command/ScaleCommand.h"
#include "Command/TranslateCommand.h"
#include "Command/RotateCommand.h"
#include "../GUIView.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include "Light.h"
#include "Imgui.h"
#include <stack>
#include <iostream>
using namespace std;

namespace sgraph
{
    /**
     * This visitor implements drawing the GUI graph using ImGUI
     *
     */
    class NodeDetailsRenderer : public SGNodeVisitor
    {
    public:
        /**
         * @brief Construct a new NodeDetailsRenderer object
         *
         * @param mv a reference to modelview stack that will be used to convert light to the view co-ordinate system
         */
        NodeDetailsRenderer(GUIView* v) : view(v)
        {
        }

        /**
         * @brief Recur to the children for drawing
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            if (ImGui::CollapsingHeader("Group Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
            }
            drawLightHeader(groupNode);
        }

        /**
         * @brief get lights attached to this leaf(if any)
         *
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {
            if (ImGui::CollapsingHeader("Leaf Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
            }
            drawLightHeader(leafNode);
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param parentNode
         */
        void visitParentNode(ParentSGNode *parentNode)
        {
            if (ImGui::CollapsingHeader("Parent Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
            }
            drawLightHeader(parentNode);
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            if (ImGui::CollapsingHeader("Transform Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
            }
            drawLightHeader(transformNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitParentNode above
         *
         * @param scaleNode
         */
        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            if (ImGui::CollapsingHeader("Scale Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
                glm::vec3 scale = scaleNode->getScale();
                float vec3f[3] = {scale.x, scale.y, scale.z};
                if(ImGui::DragFloat3("Scale", vec3f))
                {
                    // this runs only when a value is changed
                    cout<<"value changed in NodeDetailsRenderer"<<endl;
                    command::ScaleCommand* scaleCommand = new command::ScaleCommand(scaleNode->getName(), vec3f[0], vec3f[1], vec3f[2]);
                    view->addToCommandQueue(scaleCommand);
                }
            }
            drawLightHeader(scaleNode);
        }

        /**
         * @brief For this visitor, only the transformation matrix is required.
         * Thus there is nothing special to be done for each type of transformation.
         * We delegate to visitParentNode above
         *
         * @param translateNode
         */
        void visitTranslateTransform(TranslateTransform *translateNode)
        {
            if (ImGui::CollapsingHeader("Translate Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
                glm::vec3 translate = translateNode->getTranslate();
                float vec3f[3] = {translate.x, translate.y, translate.z};
                if(ImGui::DragFloat3("Translate", vec3f))
                {
                    // runs when value is changed
                    cout<<"value changed in NodeDetailsRenderer"<<endl;
                    command::TranslateCommand* translateCommand = new command::TranslateCommand(translateNode->getName(), vec3f[0], vec3f[1], vec3f[2]);
                    view->addToCommandQueue(translateCommand);
                }
            }
            drawLightHeader(translateNode);
        }

        void visitRotateTransform(RotateTransform *rotateNode)
        {
            if (ImGui::CollapsingHeader("Rotate Node", ImGuiTreeNodeFlags_DefaultOpen))
            {

                glm::vec3 rotation = rotateNode->getRotationAxis();
                float angle = glm::degrees(rotateNode->getAngleInRadians());
                float vec3f[3] = {rotation.x, rotation.y, rotation.z};
                bool changed = ImGui::DragFloat3("Rotate", vec3f);
                bool rotChanged = ImGui::InputFloat("Value", &angle, 0.1f, 1.0f);
                if(changed || rotChanged)
                {
                    command::RotateCommand* rotCommand = new command::RotateCommand(rotateNode->getName(), vec3f[0], vec3f[1], vec3f[2], glm::radians(angle));
                    view->addToCommandQueue(rotCommand);
                }
            }
            drawLightHeader(rotateNode);
        }

        void visitDynamicTransform(DynamicTransform *dynamicTransformNode)
        {
            if (ImGui::CollapsingHeader("Dynamic Node", ImGuiTreeNodeFlags_DefaultOpen))
            {
            }
            drawLightHeader(dynamicTransformNode);
        }

        void drawLightHeader(SGNode *node)
        {
            if (ImGui::CollapsingHeader("Lights", ImGuiTreeNodeFlags_DefaultOpen))
            {
                vector<util::Light> lights = node->getLights();

                for (int i = 0; i < lights.size(); i++)
                {
                    ImGui::PushID(i); // Using index as unique ID
                    glm::vec3 color = lights[i].getColor();
                    glm::vec4 spotDirection = lights[i].getSpotDirection();
                    glm::vec4 position = lights[i].getPosition();
                    float spotAngle = lights[i].getSpotCutoff();
                    if (ImGui::DragFloat3("Color", &color.x))
                    {}

                    if (ImGui::DragFloat3("Spot Direction", &spotDirection.x))
                    {}

                    if (ImGui::DragFloat3("Position", &position.x))
                    {}

                    if (ImGui::InputFloat("Angle", &spotAngle))
                    {}

                    ImGui::PopID();
                    ImGui::Separator();
                }
            }
        }

    private:

    GUIView* view;
    };
}

#endif