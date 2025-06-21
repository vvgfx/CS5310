#ifndef _SCENEGRAPHGUIRENDERER_H_
#define _SCENEGRAPHGUIRENDERER_H_

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
#include "Jobs/InsertTranslateJob.h"
#include "Jobs/InsertRotateJob.h"
#include "Jobs/InsertScaleJob.h"
using namespace std;

namespace sgraph
{
    /**
     * This visitor implements drawing the GUI graph using ImGUI
     *
     */
    class ScenegraphGUIRenderer : public SGNodeVisitor
    {
    public:
        /**
         * @brief Construct a new ScenegraphGUIRenderer object
         *
         * @param mv a reference to modelview stack that will be used to convert light to the view co-ordinate system
         */
        ScenegraphGUIRenderer(GUIView *v) : view(v) {   
            selectedNode = nullptr;
            deleteNode = nullptr;
            addChildNode = nullptr;
            nodeType = "";
        }

        /**
         * @brief Visit parent node to draw GUI.
         *
         * @param groupNode
         */
        void visitGroupNode(GroupNode *groupNode)
        {
            visitParentNode(groupNode);
        }

        /**
         * @brief draw leaf node on GUI.
         * 
         *
         * @param leafNode
         */
        void visitLeafNode(LeafNode *leafNode)
        {
            ImGuiTreeNodeFlags flags = (selectedNode == leafNode) ? ImGuiTreeNodeFlags_Selected : 0;
            flags = flags | ImGuiTreeNodeFlags_Leaf ;
            bool nodeOpen = ImGui::TreeNodeEx(leafNode->getName().c_str(), flags);
            if (ImGui::IsItemClicked())
                selectedNode = leafNode;
            if(nodeOpen)
                ImGui::TreePop();
        }

        /**
         * @brief Draw node on GUI.
         *
         * @param parentNode
         */
        void visitParentNode(ParentSGNode *parentNode)
        {
            // cout<<"Visiting node: "<<parentNode->getName()<<endl;
            ImGuiTreeNodeFlags flags = (selectedNode == parentNode) ? ImGuiTreeNodeFlags_Selected : 0;
            bool nodeOpen = ImGui::TreeNodeEx(parentNode->getName().c_str(), flags);
            if (ImGui::IsItemClicked())
                selectedNode = parentNode;
            RightClickMenu(true, parentNode);
            if(nodeOpen)
            {
                if (parentNode->getChildren().size() > 0)
                {
                    // parentNode->getChildren()[0]->accept(this);
                    for (int i = 0; i < parentNode->getChildren().size(); i = i + 1)
                    {
                        parentNode->getChildren()[i]->accept(this);
                    }
                }
                ImGui::TreePop();
            }
        }

        /**
         * @brief Visit parent node to draw GUI.
         *
         * @param transformNode
         */
        void visitTransformNode(TransformNode *transformNode)
        {
            visitParentNode(transformNode);
        }

        /**
         * @brief Visit parent node to draw GUI.
         *
         * @param scaleNode
         */
        void visitScaleTransform(ScaleTransform *scaleNode)
        {
            visitParentNode(scaleNode);
        }

        /**
         * @brief Visit parent node to draw GUI.
         *
         * @param translateNode
         */
        void visitTranslateTransform(TranslateTransform *translateNode)
        {
            visitParentNode(translateNode);
        }


        /**
         * @brief Visit parent node to draw GUI.
         *
         * @param rotateNode
         */
        void visitRotateTransform(RotateTransform *rotateNode)
        {
            visitParentNode(rotateNode);
        }

        /**
         * @brief Visit parent node to draw GUI.
         *
         * @param dynamicTransformNode
         */
        void visitDynamicTransform(DynamicTransform *dynamicTransformNode)
        {
            visitParentNode(dynamicTransformNode);
        }

        /**
         * @brief Get the node selected by the user.
         *
         */
        SGNode* getSelectedNode()
        {
            return selectedNode;
        }

        
        /**
         * This method draws the right click menu for all scenegraph nodes
         */
        void RightClickMenu(bool AddChildEnabled, SGNode* node)
        {
            // ImGui::PushID(&nodeName);
            if(ImGui::BeginPopupContextItem())
            {
                if (ImGui::BeginMenu("Add Child", AddChildEnabled))
                {
                    if(ImGui::MenuItem("Translate"))
                    {
                        view->getViewJob(new job::InsertTranslateJob(node->getName() + "-translate", 0.0, 0.0, 0.0));
                        nodeType = "Translate";
                    }
                    if(ImGui::MenuItem("Rotate"))
                    {
                        view->getViewJob(new job::InserteRotateJob(node->getName() + "-rotate", 0.0, 0.0, 0.0, 0.0));
                        nodeType = "Rotate";
                    }
                    if(ImGui::MenuItem("Scale"))
                    {
                        view->getViewJob(new job::InsertScaleJob(node->getName() + "-scale", 0.0, 0.0, 0.0));
                        nodeType = "Scale";
                    }
                    ImGui::EndMenu();
                }
                if(ImGui::MenuItem("Delete Node"))
                {
                    deleteNode = node;
                }
                ImGui::EndPopup();
            }
            // ImGui::PopID();
        }


        SGNode* getDeleteNode()
        {
            return deleteNode;
        }

        SGNode* getAddChildNode()
        {
            return addChildNode;
        }

        void resetDeleteNode()
        {
            deleteNode = nullptr;
        }

        void resetAddChildNode()
        {
            addChildNode = nullptr;
            nodeType = "";
        }


    private:

        SGNode* selectedNode;
        GUIView* view;
        SGNode* deleteNode;
        SGNode* addChildNode;
        string nodeType;
    };
}

#endif