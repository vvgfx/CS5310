#ifndef __GUIVIEW_H__
#define __GUIVIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include "View.h"
#include "sgraph/Jobs/IJob.h"
#include "GUIController.h"
using namespace std;


class GUIView : public View
{
public:
    GUIView();
    ~GUIView();
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes, map<string, util::TextureImage*> textureMap) override;
    void Resize() override;
    void display(sgraph::IScenegraph *scenegraph) override;
    bool shouldWindowClose() override;
    void closeWindow() override;
    void updateTrackball(float deltaX, float deltaY) override;
    void resetTrackball() override;
    void initScenegraphNodes(sgraph::IScenegraph *scenegraph) override;
    void changePropellerSpeed(int num) override;
    void rotatePropeller(string nodename, float time) override;
    void startRotation() override;
    void moveDrone(int direction) override;
    void setDroneOrientation(glm::mat4 resetMatrix) override;
    void rotateDrone(int yawDir, int pitchDir) override;
    void changeCameraType(int type) override;
    void initLights(sgraph::IScenegraph *scenegraph) override;
    void switchShaders() override;
    void initTextures(map<string, util::TextureImage*>& textureMap) override;

    // GUI stuff
    void ImGUIView(sgraph::IScenegraph *scenegraph);
    void GUIScenegraph(sgraph::IScenegraph *scenegraph);
    void getViewJob(job::IJob* job);
    void setControllerReference(GUIController* controller); // Ask for solutions next time :)
    void showPopups();
    
    protected: 
    void resetPopupVars();
    void initLightShaderVars() override;
    void rotate() override;
    void computeTangents(util::PolygonMesh<VertexAttrib>& mesh) override;
    sgraph::SGNodeVisitor* GuiVisitor;
    sgraph::SGNodeVisitor* NodeRenderer;

    GUIController* controller;

    // Popups
    char childNodeName[32];
    glm::vec3 newTranslation;
    glm::vec3 newRotation;
    glm::vec3 newScale;
    float newRot;
};

#endif