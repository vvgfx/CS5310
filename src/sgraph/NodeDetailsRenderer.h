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
        NodeDetailsRenderer()
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
                bool changed = ImGui::DragFloat3("Scale", vec3f);
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
                bool changed = ImGui::DragFloat3("Translate", vec3f);
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
                ImGui::InputFloat("Value", &angle, 0.1f, 1.0f);
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
                vector<util::Light> lights = node->getLights(); // Reference to avoid copy

                for (int i = 0; i < lights.size(); i++)
                {
                    ImGui::PushID(i); // Use index as unique ID instead of string labels

                    // Work directly with light data - no intermediate variables
                    glm::vec3 color = lights[i].getColor();
                    glm::vec4 spotDirection = lights[i].getSpotDirection();
                    glm::vec4 position = lights[i].getPosition();
                    float spotAngle = lights[i].getSpotCutoff();

                    // Use direct references and simple labels
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
    };
}

#endif