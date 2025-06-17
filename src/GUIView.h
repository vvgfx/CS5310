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
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes, map<string, util::TextureImage*> textureMap);
    void Resize();
    void display(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void closeWindow();
    void updateTrackball(float deltaX, float deltaY);
    void resetTrackball();
    void initScenegraphNodes(sgraph::IScenegraph *scenegraph);
    void changePropellerSpeed(int num);
    void rotatePropeller(string nodename, float time);
    void startRotation();
    void moveDrone(int direction);
    void setDroneOrientation(glm::mat4 resetMatrix);
    void rotateDrone(int yawDir, int pitchDir);
    void changeCameraType(int type);
    void initLights(sgraph::IScenegraph *scenegraph);
    void switchShaders();
    void initTextures(map<string, util::TextureImage*>& textureMap);
    
    protected: 
    void initLightShaderVars();
    void rotate();
    void computeTangents(util::PolygonMesh<VertexAttrib>& mesh);
};

#endif