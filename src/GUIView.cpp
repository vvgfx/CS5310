#include "GUIView.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/LightRetriever.h"
#include "sgraph/ShadowRenderer.h"
#include "sgraph/DepthRenderer.h"
#include "sgraph/AmbientRenderer.h"
#include "sgraph/ScenegraphGUIRenderer.h"
#include "sgraph/NodeDetailsRenderer.h"
#include "VertexAttrib.h"
#include "Pipeline/ShadowVolumePipeline.h"
#include "Pipeline/ShadowVolumeBumpMappingPipeline.h"
#include "Pipeline/BasicPBRPipeline.h"
#include "Pipeline/TexturedPBRPipeline.h"
#include "Pipeline/PBRShadowVolumePipeline.h"
#include "Pipeline/TexturedPBRSVPipeline.h"
#include "sgraph/Jobs/InsertGroupJob.h"
#include "sgraph/Jobs/InsertRotateJob.h"
#include "sgraph/Jobs/InsertTranslateJob.h"
#include "sgraph/Jobs/InsertScaleJob.h"
#include "sgraph/Jobs/InsertLeafJob.h"
#include "sgraph/Jobs/DeleteNodeJob.h"

// Imgui required files.
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

GUIView::GUIView()
{
    resetPopupVars();
}

GUIView::~GUIView()
{
}

void GUIView::init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage *> texMap)
{
    View::init(callbacks, meshes, texMap);
    cout<<"Setting up ImGUI initialization"<<endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io; // suppress compiler warnings
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // enable keyboard navigation

    ImGui::StyleColorsDark(); // dark mode
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    GuiVisitor = new sgraph::ScenegraphGUIRenderer(this);
    NodeRenderer = new sgraph::NodeDetailsRenderer(this);
}

void GUIView::initTextures(map<string, util::TextureImage *> &textureMap)
{
    View::initTextures(textureMap);
}

void GUIView::computeTangents(util::PolygonMesh<VertexAttrib> &tmesh)
{
    View::computeTangents(tmesh);
}

void GUIView::Resize()
{
    View::Resize();
}

void GUIView::updateTrackball(float deltaX, float deltaY)
{
    View::updateTrackball(deltaX, deltaY);
}

void GUIView::resetTrackball()
{
    View::resetTrackball();
}

void GUIView::display(sgraph::IScenegraph *scenegraph)
{
    #pragma region lightSetup
    // setting up the view matrices beforehand because all render calculations are going to be on the view coordinate system.

    glm::mat4 viewMat(1.0f);
    if(cameraType == 1)
        viewMat = viewMat * glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    else if(cameraType == 2)
        viewMat = viewMat * glm::lookAt(glm::vec3(0.0f, 150.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    else if(cameraType == 3)
    {
            //Drone camera. Need to find a point that is forward(for the lookAt), find the drone co-ordinates(for the eye) and the up-direction for the up-axis
            //drone co-ordinates seem simple enough. I can just use the transform matrix with a translation of 20 in the z-axis
            //target = same as eye, the translation must be higher, so 25?
            //for the up-axis, I can convert the y axis to vec4, pre-multiply by the transformation matrix, then convert back to vec3.
            //This seems super hacky though, is there an alternate way that's easier?
            
            glm::mat4 droneTransformMatrix = dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["drone-movement"])->getTransformMatrix();
            glm::vec3 droneEye = droneTransformMatrix * glm::vec4(0.0f, 0.0f, 20.0f, 1.0f); // setting 1 as the homogenous coordinate
            // Implicit typecasts work!!!!
            glm::vec3 droneLookAt = droneTransformMatrix * glm::vec4(0.0f, 0.0f, 25.0f, 1.0f);
            glm::vec3 droneUp = droneTransformMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);//homogenous coordinate is 0.0f as the vector is an axis, not a point.
            
            viewMat = viewMat * glm::lookAt(droneEye, droneLookAt, droneUp);        
    }
    #pragma endregion


    #pragma region pipeline

    pipeline->drawFrame(scenegraph, viewMat);

    #pragma endregion
    

    // Draw GUI here

    ImGUIView(scenegraph);

    
    
    
    glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
}

void GUIView::ImGUIView(sgraph::IScenegraph *scenegraph)
{
    ImGuiIO& io = ImGui::GetIO(); (void)io; // change this later

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("GUI Test!");

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::End();
    GUIScenegraph(scenegraph);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GUIView::GUIScenegraph(sgraph::IScenegraph *scenegraph)
{

    // Draw the scenegraph on the left
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 8 , ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
    ImGui::Begin("Scenegraph");
    scenegraph->getRoot()->accept(GuiVisitor);
    ImGui::End();

    // Draw the Node details on the right
    sgraph::SGNode* selectedNode = reinterpret_cast<sgraph::ScenegraphGUIRenderer*>(GuiVisitor)->getSelectedNode();
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x  * 5 / 6, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x / 6 , ImGui::GetIO().DisplaySize.y), ImGuiCond_Always);
    ImGui::Begin("Node Details");
    if(selectedNode)
    selectedNode->accept(NodeRenderer);

    // These are for the popups. At any point, I expect only one at most to have a popup
    ImGui::End();

    showPopups();

}

/**
 * This is ugly. Is there an alternative to this method?
 */
void GUIView::showPopups()
{
    sgraph::ScenegraphGUIRenderer* sgRenderer = reinterpret_cast<sgraph::ScenegraphGUIRenderer*>(GuiVisitor);
    sgraph::SGNode* deleteNode = sgRenderer->getDeleteNode();
    if(deleteNode!= nullptr)
    {
        ImGui::OpenPopup("Delete Node?");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if(ImGui::BeginPopupModal("Delete Node?"))
        {
            ImGui::Text("This node will be deleted.\nThis operation cannot be undone!");
            ImGui::Separator();
            if (ImGui::Button("Yes")) 
            {
                job::DeleteNodeJob* deleteNodeJob = new job::DeleteNodeJob(deleteNode->getName(), deleteNode->getParent()->getName());
                getViewJob(deleteNodeJob);
                ImGui::CloseCurrentPopup();
                sgRenderer->resetDeleteNode();
            }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("No")) 
            {
                ImGui::CloseCurrentPopup();
                sgRenderer->resetDeleteNode();
            }
            ImGui::EndPopup();
        }
    }

    sgraph::SGNode* addChildNode = sgRenderer->getAddChildNode();
    if(addChildNode != nullptr)
    {
        // show popup for add child
        string childType = sgRenderer->getNodeType();
        if(childType == "Translate")
        {
            ImGui::OpenPopup("Add Translate Node");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            

            if(ImGui::BeginPopupModal("Add Translate Node"))
            {
                ImGui::InputText("Node name", childNodeName, 32);
                float translateFloat3f[3] = {newTranslation.x, newTranslation.y, newTranslation.z};
                bool translateChanged = ImGui::DragFloat3("Translate", translateFloat3f);
                if (translateChanged)
                {
                    newTranslation.x = translateFloat3f[0];
                    newTranslation.y = translateFloat3f[1];
                    newTranslation.z = translateFloat3f[2];
                }
                ImGui::Separator();
                if (ImGui::Button("Confirm")) 
                {
                    job::InsertTranslateJob* translateJob = new job::InsertTranslateJob( sgRenderer->getAddChildNode()->getName() , childNodeName, newTranslation.x, newTranslation.y, newTranslation.z);
                    getViewJob(translateJob);
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) 
                {
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::EndPopup();
            }
        }
        else if(childType == "Rotate")
        {
            ImGui::OpenPopup("Add Rotate Node");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            

            if(ImGui::BeginPopupModal("Add Rotate Node"))
            {
                ImGui::InputText("Node name", childNodeName, 32);
                float newRotation3f[3] = {newRotation.x, newRotation.y, newRotation.z};
                bool rotateChanged = ImGui::DragFloat3("Axis", newRotation3f);
                bool rotChanged = ImGui::InputFloat("Rotation", &newRot, 0.1f, 1.0f);
                if (rotateChanged)
                {
                    newRotation.x = newRotation3f[0];
                    newRotation.y = newRotation3f[1];
                    newRotation.z = newRotation3f[2];
                }
                ImGui::Separator();
                if (ImGui::Button("Confirm")) 
                {
                    job::InserteRotateJob* rotateJob = new job::InserteRotateJob(sgRenderer->getAddChildNode()->getName(), childNodeName, newRotation.x, newRotation.y, newRotation.z, newRot);
                    getViewJob(rotateJob);
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) 
                {
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::EndPopup();
            }
        }
        else if(childType == "Scale")
        {
            ImGui::OpenPopup("Add Scale Node");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            

            if(ImGui::BeginPopupModal("Add Scale Node"))
            {
                ImGui::InputText("Node name", childNodeName, 32);
                float scaleFloat3f[3] = {newScale.x, newScale.y, newScale.z};
                bool scaleChanged = ImGui::DragFloat3("Scale", scaleFloat3f);
                if (scaleChanged)
                {
                    newScale.x = scaleFloat3f[0];
                    newScale.y = scaleFloat3f[1];
                    newScale.z = scaleFloat3f[2];
                }
                ImGui::Separator();
                if (ImGui::Button("Confirm")) 
                {
                    job::InsertScaleJob* scaleJob = new job::InsertScaleJob(sgRenderer->getAddChildNode()->getName(), childNodeName, newScale.x, newScale.y, newScale.z);
                    getViewJob(scaleJob);
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) 
                {
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::EndPopup();
            }
        }
        else if(childType == "Group")
        {
            ImGui::OpenPopup("Add Group Node");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal("Add Group Node"))
            {
                ImGui::InputText("Node name", childNodeName, 32);
                ImGui::Separator();
                if (ImGui::Button("Confirm")) 
                {
                    job::InsertGroupJob* groupJob = new job::InsertGroupJob(sgRenderer->getAddChildNode()->getName(), childNodeName);
                    getViewJob(groupJob);
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) 
                {
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::EndPopup();
            }
        }
        else if(childType == "Leaf")
        {
            ImGui::OpenPopup("Add Leaf Node");

            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
            if(ImGui::BeginPopupModal("Add Leaf Node"))
            {
                ImGui::InputText("Node name", childNodeName, 32);
                ImGui::InputText("Object instance", objectInstanceName, 32);

                // float albedoFloat3f[3] = {leafAlbedo.x, leafAlbedo.y, leafAlbedo.z};
                // if(ImGui::DragFloat3("Albedo", albedoFloat3f, 0.01f, 0.0f, 1.0f))
                // {
                //     leafAlbedo.x = albedoFloat3f[0];
                //     leafAlbedo.y = albedoFloat3f[1];
                //     leafAlbedo.z = albedoFloat3f[2];
                // }

                ImVec4 colorAlbedo = ImVec4(leafAlbedo.x, leafAlbedo.y, leafAlbedo.z, 1.0f);
                if(ImGui::ColorEdit3("clear color", (float*)&colorAlbedo))
                {
                    leafAlbedo.x = colorAlbedo.x;
                    leafAlbedo.y = colorAlbedo.y;
                    leafAlbedo.z = colorAlbedo.z;
                };

                bool metallicChanged = ImGui::SliderFloat("Metallic", &materialMetallic, 0.1f, 1.0f);
                bool roughnessChanged = ImGui::SliderFloat("Roughness", &materialRoughness, 0.1f, 1.0f);
                bool aoChanged = ImGui::SliderFloat("Ambient Occlusion", &materialAO, 0.1f, 1.0f);
                
                ImGui::Separator();
                if (ImGui::Button("Confirm")) 
                {
                    job::InsertLeafJob* leafJob = new job::InsertLeafJob(sgRenderer->getAddChildNode()->getName(), childNodeName, leafAlbedo, materialMetallic, materialRoughness, materialAO, objectInstanceName, ""); //passing texture as empty
                    getViewJob(leafJob);
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::SetItemDefaultFocus();
                ImGui::SameLine();
                if (ImGui::Button("Cancel")) 
                {
                    ImGui::CloseCurrentPopup();
                    sgRenderer->resetAddChildNode();
                    resetPopupVars();
                }
                ImGui::EndPopup();
            }
        }
    }
}

void GUIView::resetPopupVars()
{
    strcpy(childNodeName, ""); // reset char array
    newTranslation = glm::vec3(0.0f);
    newRotation = glm::vec3(0.0f);
    newScale = glm::vec3(0.0f);
    newRot = 0.0f;

    strcpy(objectInstanceName, ""); // reset char array
    strcpy(materialName, ""); // reset char array
    leafAlbedo = glm::vec3(0.0f);
    materialMetallic = 0.0f;
    materialRoughness = 0.0f;
    materialAO = 0.0f;
}

void GUIView::initLights(sgraph::IScenegraph *scenegraph)
{
    View::initLights(scenegraph);
}

void GUIView::initLightShaderVars()
{
    View::initLightShaderVars();
}

bool GUIView::shouldWindowClose()
{
    return View::shouldWindowClose();
}

void GUIView::switchShaders()
{
    // not supported
}

void GUIView::closeWindow()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    View::closeWindow();
}

// This saves all the nodes required for dynamic transformation
void GUIView::initScenegraphNodes(sgraph::IScenegraph *scenegraph)
{
    View::initScenegraphNodes(scenegraph);
}

void GUIView::rotatePropeller(string nodeName, float time)
{
    // not supported
}

void GUIView::changePropellerSpeed(int num)
{
    // not supported
}

void GUIView::startRotation()
{
    // not supported
}

void GUIView::rotate()
{
    // not supported
}

/**
 * Move/Rotate the drone by passing the matrix to premultiply. This stacks on top of previous input.
 */
void GUIView::moveDrone(int direction)
{
    // not supported
}

/**
 * Set the drone's matrix to what is passed. A good idea would be to pass a rotation and a translation.
 */
void GUIView::setDroneOrientation(glm::mat4 resetMatrix)
{
    // not supported
}

/**
 * Pass a positive yawDir to rotate left, negative to rotate right. Similarly, positive pitchDir to rotate upwards, negative to rotate downwards
 */

void GUIView::rotateDrone(int yawDir, int pitchDir)
{
    // not supported
}

void GUIView::changeCameraType(int cameraType)
{
    // not supported
}

void GUIView::getViewJob(job::IJob* job)
{
    controller->receiveJob(job);
}

void GUIView::setControllerReference(GUIController* controller)
{
    this->controller = controller;
}