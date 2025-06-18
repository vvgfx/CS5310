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
        NodeDetailsRenderer() {   
        }

        /**
         * @brief Recur to the children for drawing
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            ImGui::Text("Group Node");
            
        }

        /**
         * @brief get lights attached to this leaf(if any)
         * 
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {
            ImGui::Text("Leaf Node");
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param parentNode
         */
        void visitParentNode(ParentSGNode *parentNode)
        {
           ImGui::Text("Parent Node");
        }

        /**
         * @brief Multiply the transform to the modelview and recur to child
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            ImGui::Text("Transform Node");
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
            ImGui::Text("Scale Node");

            glm::vec3 scale = scaleNode->getScale();
            float vec3f[3] = {scale.x , scale.y, scale.z};
            bool changed = ImGui::DragFloat3("Scale", vec3f);
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
            ImGui::Text("Translate Node");

            glm::vec3 translate = translateNode->getTranslate();
            float vec3f[3] = {translate.x , translate.y, translate.z};
            bool changed = ImGui::DragFloat3("Translation", vec3f);
        }

        void visitRotateTransform(RotateTransform *rotateNode)
        {
            ImGui::Text("Rotate Node");


            glm::vec3 rotation = rotateNode->getRotationAxis();
            float angle = glm::degrees(rotateNode->getAngleInRadians());
            float vec3f[3] = {rotation.x , rotation.y, rotation.z};
            bool changed = ImGui::DragFloat3("Rotation", vec3f);
            ImGui::InputFloat("Value", &angle, 0.1f, 1.0f);
        }

        void visitDynamicTransform(DynamicTransform *dynamicTransformNode)
        {
            ImGui::Text("Dynamic Node");
        }

    private:

    };
}

#endif