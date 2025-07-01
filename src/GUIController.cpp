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
#include <thread>
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
        inFile = ifstream("scenegraphmodels/texture-pbr-shadow-volume-test-2.txt");
    else
        inFile = ifstream(textfile);
    sgraph::ScenegraphImporter importer;
    

    IScenegraph *scenegraph = importer.parse(inFile);
    //scenegraph->setMeshes(meshes);
    model->setScenegraph(scenegraph);
    map<string, util::TextureImage*> textureMap = importer.getTextureMap(); // this is copied

    model->saveTextureMap(textureMap); // this should also be copied, the source of truth is the model!

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
    sgraph::IScenegraph * scenegraph = model->getScenegraph();
    map<string,util::PolygonMesh<VertexAttrib> > meshes = scenegraph->getMeshes();

    map<string, unsigned int>& texIdMap = model->getTextureIdMap(); // not copied! reference to the model variable.
    view->init(this,meshes, texIdMap);
    // creating the texture Id maps AFTER init. This is because the OpenGL initialization needs to occur before the textures can be loaded
    model->initTextures(model->getTextureMap());
    //Save the nodes required for transformation when running!
    view->initScenegraphNodes(scenegraph);
    while (!view->shouldWindowClose()) {
        model->clearQueues();
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

void GUIController::receiveJob(job::IJob* job)
{
    cout<<"received job in controller"<<endl;
    // job->execute(model);
    thread t([this, job]()
    {
        job->execute(model);
    });
    t.detach();
    
}