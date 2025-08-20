#ifndef __GUIVIEW_H__
#define __GUIVIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include "View.h"
#include "sgraph/Jobs/IJob.h"
#include "GUICallbacks.h"
#include "Camera/ICamera.h"
using namespace std;
class GUIView : public View
{
public:
    GUIView();
    ~GUIView();
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes, map<string, unsigned int>& texIdMap, sgraph::IScenegraph* sgraph) override;
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

    void pipelineCallbacks(int key);

    // GUI stuff
    void ImGUIView(sgraph::IScenegraph *scenegraph);
    void GUIScenegraph(sgraph::IScenegraph *scenegraph);
    void getViewJob(job::IJob* job);
    void setGUICallbackReference(GUICallbacks* controller); // Ask for solutions next time :)
    void showPopups();
    void guiSwitch();

    // camera
    void moveCamera(int forwardDir, int horizontalDr);
    void rotateCamera(int xDir, int yDir);

    // runtime meshes
    void loadMesh(string meshName, util::PolygonMesh<VertexAttrib>& polymesh);

    // cubemap stuff here
    void loadCubeMaps(vector<util::TextureImage*>& cubeMap);
    
    protected: 
    void resetPopupVars();
    void initLightShaderVars() override;
    void rotate() override;
    void computeTangents(util::PolygonMesh<VertexAttrib>& mesh) override;
    sgraph::SGNodeVisitor* GuiVisitor;
    sgraph::SGNodeVisitor* NodeRenderer;

    GUICallbacks* callbacks;

    // Popups
    char childNodeName[32];
    glm::vec3 newTranslation;
    glm::vec3 newRotation;
    glm::vec3 newScale;
    float newRot;


    char objectInstanceName[32];
    char materialName[32];
    glm::vec3 leafAlbedo;
    float materialMetallic;
    float materialRoughness;
    float materialAO;
    bool leafTextures;
    char albedoMap[100];
    char normalMap[100];
    char metallicMap[100];
    char roughnessMap[100];
    char aoMap[100];
    char texName[100];
    char texPath[100];
    bool loadTexture;

    bool showGui = true;

    bool loadModel;
    char modelName[100];
    char modelPath[100];

    // testing alternative camera
    camera::ICamera* camera;
    float lastFrame;
    float deltaTime;

    // load scenegraph
    char newFileName[200];
    bool showLoadScenePopup;

    // save scenegraph
    char saveFileName[200];
    bool showSaveScenePopup;
};

#endif