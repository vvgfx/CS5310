#include "Controller.h"
#include "sgraph/IScenegraph.h"
#include "sgraph/Scenegraph.h"
#include "sgraph/GroupNode.h"
#include "sgraph/LeafNode.h"
#include "sgraph/ScaleTransform.h"
#include "ObjImporter.h"
using namespace sgraph;
#include <iostream>
using namespace std;

#include "sgraph/ScenegraphExporter.h"
#include "sgraph/ScenegraphImporter.h"
#include "sgraph/ScenegraphDrawer.h"

Controller::Controller(Model& m,View& v, string textfile) :
    mousePressed(false) {
    model = m;
    view = v;
    this->textfile = textfile;
    initScenegraph();
}

void Controller::initScenegraph() {

     
    
    //read in the file of commands
    ifstream inFile;
    if(textfile == "")
        inFile = ifstream("scenegraphmodels/big-ben.txt");
    else
        inFile = ifstream(textfile);
    //ifstream inFile("tryout.txt");
    sgraph::ScenegraphImporter importer;
    

    IScenegraph *scenegraph = importer.parse(inFile);
    //scenegraph->setMeshes(meshes);
    model.setScenegraph(scenegraph);
    cout <<"Scenegraph made" << endl;   
    sgraph::ScenegraphDrawer* drawer = new sgraph::ScenegraphDrawer();
    scenegraph->getRoot()->accept(drawer);
}

Controller::~Controller()
{
    
}

void Controller::run()
{
    sgraph::IScenegraph * scenegraph = model.getScenegraph();
    map<string,util::PolygonMesh<VertexAttrib> > meshes = scenegraph->getMeshes();
    view.init(this,meshes);
    //Save the nodes required for transformation when running!
    view.initScenegraphNodes(scenegraph);
    //Set the initial orientation of the drone!
    glm::mat4 droneOrientation  = glm::translate(glm::mat4(1.0f), glm::vec3(-100.0f, 100.0f, 150.0f));
    view.setDroneOrientation(droneOrientation);
    while (!view.shouldWindowClose()) {
        view.display(scenegraph);
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
    cout << (char)key << " pressed" << endl;

    if(action != GLFW_PRESS && action != GLFW_REPEAT)
        return;
    switch(key)
    {
        case GLFW_KEY_R:
            view.resetTrackball();
            break;
        case GLFW_KEY_S:
            view.changePropellerSpeed(-1);
            break;
        case GLFW_KEY_F:
            view.changePropellerSpeed(1);
            break;
        case GLFW_KEY_Z:
            view.startRotation();
            break;
        case GLFW_KEY_LEFT:
            view.moveDrone(glm::rotate(glm::mat4(1.0f), glm::radians(5.0f) , glm::vec3(0.0f, 1.0f, 0.0f)));
            break;
        case GLFW_KEY_RIGHT:
            view.moveDrone(glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f) , glm::vec3(0.0f, 1.0f, 0.0f)));
            break;
        case GLFW_KEY_UP:
            view.moveDrone(glm::rotate(glm::mat4(1.0f), glm::radians(5.0f) , glm::vec3(1.0f, 0.0f, 0.0f)));
            break;
        case GLFW_KEY_DOWN:
            view.moveDrone(glm::rotate(glm::mat4(1.0f), glm::radians(-5.0f) , glm::vec3(1.0f, 0.0f, 0.0f)));
            break;
        case GLFW_KEY_EQUAL:
            view.moveDrone(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 5.0f)));
            break;
        case GLFW_KEY_MINUS:
            view.moveDrone(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -5.0f)));
            break;
    }
}

void Controller::onMouseInput(int button, int action, int mods)
{
    if(button != GLFW_MOUSE_BUTTON_LEFT)
        return;
    mousePressed = action == GLFW_PRESS;
    string mouseStatus = mousePressed ? "mouse pressed!" : "mouse released!";
    double xPos, yPos;
    cout<<mouseStatus<<endl;
}

void Controller::onCursorMove(double newXPos, double newYPos)
{
    oldXPos = this->newXPos;
    oldYPos = this->newYPos;
    this->newXPos = newXPos;
    this->newYPos = newYPos;
    float deltaX = newXPos - oldXPos;
    float deltaY = newYPos - oldYPos;
    if(!(mousePressed && ( deltaX != 0 || deltaY != 0)))
    return;
    float sensitivity = 0.005f;
    glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0), (deltaX * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));
    rotMatrix = glm::rotate(rotMatrix, (deltaY * sensitivity), glm::vec3(1.0f, 0.0f, 0.0f));
    cout<<"Direction: "<<newXPos - oldXPos<<" , "<<newYPos - oldYPos<<endl;
    view.updateTrackball(rotMatrix);
}

void Controller::reshape(int width, int height) 
{
    cout <<"Window reshaped to width=" << width << " and height=" << height << endl;
    glViewport(0, 0, width, height);
    view.Resize();
}

void Controller::dispose()
{
    view.closeWindow();
}

void Controller::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}