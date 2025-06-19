#include "GUIController.h"
#include "sgraph/ImageLoader.h"
#include "sgraph/PPMImageLoader.h"
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
#include "GUIView.h"

GUIController::GUIController(Model* m,View* v, string textfile) :
    Controller(m,v, textfile) {
}

void GUIController::initScenegraph() {

     
    
    //read in the file of commands
    ifstream inFile;
    if(textfile == "")
        inFile = ifstream("scenegraphmodels/pbr-shadow-volume-test.txt");
    else
        inFile = ifstream(textfile);
    sgraph::ScenegraphImporter importer("textures/white.ppm");
    

    IScenegraph *scenegraph = importer.parse(inFile);
    //scenegraph->setMeshes(meshes);
    model->setScenegraph(scenegraph);
    map<string, util::TextureImage*> textureMap = importer.getTextureMap();
    model->saveTextureMap(textureMap);

    map<string, sgraph::SGNode*> nodes = importer.getNodeMap();
    model->saveNodes(nodes);

    // ugly code, can fix later??
    reinterpret_cast<GUIView*>(view)->setControllerReference(this);

    cout <<"Scenegraph made in GUIController" << endl;   
    sgraph::ScenegraphDrawer* drawer = new sgraph::ScenegraphDrawer();
    scenegraph->getRoot()->accept(drawer);
}

GUIController::~GUIController()
{
    
}

void GUIController::run()
{
    cout<<"Child controller run"<<endl;
    sgraph::IScenegraph * scenegraph = model->getScenegraph();
    map<string,util::PolygonMesh<VertexAttrib> > meshes = scenegraph->getMeshes();
    map<string, util::TextureImage*> texMap = model->getTextureMap();
    view->init(this,meshes, texMap);
    //Save the nodes required for transformation when running!
    view->initScenegraphNodes(scenegraph);
    //Set the initial orientation of the drone!
    while (!view->shouldWindowClose()) {
        model->clearCommandQueue();
        view->display(scenegraph);
    }
    view->closeWindow();
    exit(EXIT_SUCCESS);
}

void GUIController::onkey(int key, int scancode, int action, int mods)
{
    cout << (char)key << " pressed on GUIController" << endl;
}

void GUIController::onMouseInput(int button, int action, int mods)
{
    Controller::onMouseInput(button, action, mods);
}

void GUIController::onCursorMove(double newXPos, double newYPos)
{
    Controller::onCursorMove(newXPos, newYPos);
}

void GUIController::reshape(int width, int height) 
{
    Controller::reshape(width, height);
}

void GUIController::dispose()
{
    Controller::dispose();
}

void GUIController::error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void GUIController::addToCommandQueue(command::ICommand* command)
{
    cout<<"controller adding to command queue"<<endl;
    model->addToCommandQueue(command);
}