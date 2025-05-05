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

Controller::Controller(Model& m,View& v, string textfile) :cameraPos(200.0f, 250.0f, 250.0f),
    target(0.0f, 0.0f, 0.0f),
    up(0.0f, 1.0f, 0.0f),
    mousePressed(false) {
    model = m;
    view = v;
    radius = glm::length(cameraPos - target);
    theta = atan2(cameraPos.z, cameraPos.x);
    phi = asin(cameraPos.y / radius);

    // store init values for reset  
    initialCameraPos = cameraPos;
    initialTheta = theta;
    initialPhi = phi;
    this->textfile = textfile;
    initScenegraph();
}

void Controller::initScenegraph() {

     
    
    //read in the file of commands
    ifstream inFile;
    if(textfile == "")
        inFile = ifstream("scenegraphmodels/drone.txt");
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
    while (!view.shouldWindowClose()) {
        view.setLookAt(glm::lookAt(cameraPos, target, up));
        view.display(scenegraph);
    }
    view.closeWindow();
    exit(EXIT_SUCCESS);
}

void Controller::onkey(int key, int scancode, int action, int mods)
{
    cout << (char)key << " pressed" << endl;
    if (key == 82) //r
    {
        this->newXPos = newXPos;
        this->newYPos = newYPos;
        view.xDelta = 0.0f;
        view.yDelta = 0.0f;
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
    theta -= (deltaX * sensitivity);
    phi -= (deltaY * sensitivity);
    // cout<<"Direction: "<<newXPos - oldXPos<<" , "<<newYPos - oldYPos<<endl;
    updateCameraPosition();
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

void Controller::updateCameraPosition() {
    
    // manually apply rotations to avoid gimbal lock
    glm::mat4 rotation = glm::mat4(1.0f);

    // Rotate around the Y
    rotation = glm::rotate(rotation, theta, glm::vec3(0.0f, 1.0f, 0.0f));

    // Rotate around the X
    rotation = glm::rotate(rotation, phi, glm::vec3(1.0f, 0.0f, 0.0f));

    // Initial z
    glm::vec4 initialPos(0.0f, 0.0f, radius, 1.0f);

    // Apply rotation and update camera
    glm::vec4 transformedPos = rotation * initialPos;
    cameraPos = target + glm::vec3(transformedPos);

    // Compute up vector
    glm::vec3 newUp = glm::vec3(rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    up = newUp;
}