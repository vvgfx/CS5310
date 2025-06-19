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

// Imgui required files.
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

GUIView::GUIView()
{
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

    GuiVisitor = new sgraph::ScenegraphGUIRenderer();
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
    ImGui::End();

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

void GUIView::addToCommandQueue(command::ICommand* command)
{
    controller->addToCommandQueue(command);
}

void GUIView::setControllerReference(GUIController* controller)
{
    this->controller = controller;
}