#ifndef __VIEW_H__
#define __VIEW_H__

#include <glad/glad.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <cstdio>
#include <ShaderProgram.h>
#include <ObjectInstance.h>
#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "Callbacks.h"


class View
{
public:
    View();
    ~View();
    void init(Callbacks* callbacks,vector<util::PolygonMesh<VertexAttrib> >& meshes);
    void display();
    bool shouldWindowClose();
    void closeWindow();

private:   

    GLFWwindow* window;
    util::ShaderProgram program;
    util::ShaderLocationsVault shaderLocations;
    vector<util::ObjectInstance *> objects;
    glm::mat4 modelview,projection;
    int frames;
    double time;
};

#endif