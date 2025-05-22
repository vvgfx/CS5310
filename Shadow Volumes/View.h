#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderGeoProgram.h> // This is for silhouettes/shadow volumes/anything that requires geometry shaders.
#include <ShaderProgram.h> // This is for normal rendering
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"
#include "TextureImage.h"
#include <stack>
using namespace std;


class View
{
public:
    View();
    ~View();
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
    void initShaderVars();
    void initLights(sgraph::IScenegraph *scenegraph);
    void switchShaders();
    void initTextures(map<string, util::TextureImage*> textureMap);
    float xDelta, yDelta, zDelta;

    //This class saves the shader locations of all the light inputs.
    class LightLocation 
    {

    public:
        int ambient,diffuse,specular,position;
        int spotDirection, spotAngle;
        LightLocation()
        {
            ambient = diffuse = specular = position = -1;
            spotDirection = spotAngle = -1;
        }

    };

private: 
    void rotate();
    GLFWwindow* window;
    util::ShaderProgram renderProgram;
    util::ShaderGeoProgram silhouetteProgram;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    sgraph::SGNodeVisitor *lightRetriever;
    int frames;
    double time;
    float rotationSpeed = 0.5f;
    float speed = 1.0f;

    bool isRotating = false;
    float rotationAngle = 0.0f;

    int cameraType = 1;

    //Saving all the required nodes for dynamic transformation!
    std::map<string, sgraph::TransformNode*> cachedNodes; // Need to save this as a pointer because TransformNode is abstract :(
    vector<LightLocation> lightLocations;
    vector<util::Light> lights;
    vector<glm::mat4> lightTransformations;
    bool isToonShaderUsed = false;
    map<string, unsigned int> textureIdMap;
};

#endif