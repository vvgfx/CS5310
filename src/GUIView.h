#ifndef __GUIVIEW_H__
#define __GUIVIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include "View.h"
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// #include <cstdio>
// #include <ShaderGeoProgram.h> // This is for silhouettes/shadow volumes/anything that requires geometry shaders.
// #include <ShaderProgram.h> // This is for normal rendering
// #include "sgraph/SGNodeVisitor.h"
// #include "ObjectInstance.h"
// #include "PolygonMesh.h"
// #include "VertexAttrib.h"
// #include "Callbacks.h"
// #include "sgraph/IScenegraph.h"
// #include "TextureImage.h"
// #include <stack>
// #include "Pipeline/ClassicPipeline.h"
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
    void ImGUIView();
    
    protected: 
    void initLightShaderVars() override;
    void rotate() override;
    void computeTangents(util::PolygonMesh<VertexAttrib>& mesh) override;
};

#endif