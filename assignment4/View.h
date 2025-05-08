#ifndef __VIEW_H__
#define __VIEW_H__

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include "sgraph/SGNodeVisitor.h"
#include "ObjectInstance.h"
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"
#include "sgraph/IScenegraph.h"

#include <stack>
using namespace std;


class View
{
public:
    View();
    ~View();
    void init(Callbacks* callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes);
    void Resize();
    void display(sgraph::IScenegraph *scenegraph);
    bool shouldWindowClose();
    void closeWindow();
    void updateTrackball(glm::mat4 updateMatrix);
    void resetTrackball();
    void initScenegraphNodes(sgraph::IScenegraph *scenegraph);
    void changePropellerSpeed(int num);
    void rotatePropeller(string nodename, float time);
    void startRotation();
    void moveDrone(int direction);
    void setDroneOrientation(glm::mat4 resetMatrix);
    void rotateDrone(int yawDir, int pitchDir);
    float xDelta, yDelta, zDelta;

private: 
    void rotate();
    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    map<string,util::ObjectInstance *> objects;
    glm::mat4 projection;
    stack<glm::mat4> modelview;
    sgraph::SGNodeVisitor *renderer;
    int frames;
    double time;
    float rotationSpeed = 0.5f;
    float speed = 1.0f;

    bool isRotating = false;
    float rotationAngle = 0.0f;

    //Saving all the required nodes for dynamic transformation!
    std::map<string, sgraph::TransformNode*> cachedNodes; // Need to save this as a pointer because TransformNode is abstract :(
};

#endif