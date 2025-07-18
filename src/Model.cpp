#include "Model.h"
#include "sgraph/SGNode.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <thread>
#include <mutex>

Model::Model() {

}

Model::~Model() {
    if (scenegraph) {
        delete scenegraph;
    }
}

sgraph::IScenegraph *Model::getScenegraph() {
    return this->scenegraph;
}

void Model::setScenegraph(sgraph::IScenegraph *scenegraph) {
    this->scenegraph = scenegraph;
}

void Model::saveTextureMap(map<string, util::TextureImage*> texMap)
{
    this->textureMap = texMap;
}

map<string, util::TextureImage*>& Model::getTextureMap()
{
    return this->textureMap;
}

void Model::addToCommandQueue(command::ICommand* command)
{
    cout<<"Adding to command queue"<<endl;
    commandMutex.lock();
    backCommandQueue.push(command);
    commandMutex.unlock();
}

void Model::clearQueues()
{
    // clear tasks first
    if(!backTaskQueue.empty())
    {
        cout<<"Task queue is not empty!"<<endl;
        taskMutex.lock();
        std::swap(backTaskQueue, frontTaskQueue);
        taskMutex.unlock();
        while(!frontTaskQueue.empty())
        {
            task::ITask* task = frontTaskQueue.front();
            task->execute();
            frontTaskQueue.pop();
            delete task;
        }
    }

    // clear command queues now.
    if(backCommandQueue.empty())
        return;
    commandMutex.lock();
    std::swap(backCommandQueue, frontCommandQueue);
    commandMutex.unlock();
    while(!frontCommandQueue.empty())
    {
        cout<<"model queue is not empty : "<<frontCommandQueue.size()<<endl;
        command::ICommand* command = frontCommandQueue.front();
        string nodeName = command->getNodeName();
        map<string, sgraph::SGNode *>* nodes = scenegraph->getNodes();
        if(nodes->find(nodeName) != nodes->end())
        {
            // node exists in hierarchy
            (*nodes)[nodeName]->accept(command);
        }
        frontCommandQueue.pop();
        delete command;
    }
}

void Model::addToTaskQueue(task::ITask* task)
{
    cout<<"Adding to task queue"<<endl;
    taskMutex.lock();
    backTaskQueue.push(task);
    taskMutex.unlock();

}

// This needs to run AFTER the view's init because the opengl context and references are set up there
void Model::initTextures(map<string, util::TextureImage*>& textureMap)
{
    for(typename map<string, util::TextureImage*>::iterator it = textureMap.begin(); it!=textureMap.end(); it++)
    {
        //first - name of texture, second - texture itself
        util::TextureImage* textureObject = it->second;
        //generate texture ID
        unsigned int textureId;
        glGenTextures(1,&textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);
        //texture params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Mipmaps are not available for maximization
        
        //copy texture to GPU
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, textureObject->getWidth(),textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,textureObject->getImage()); // TODO: create a new map for GL_RGB16F images here. Might be useful later
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //save id in map
        textureIdMap[it->first] = textureId;
        
    }
}

/**
 * Since this code does not use mutexes, run it on the main thread. Use the task interface if you need to.
 */
void Model::addTexture(string name, string path, util::TextureImage* textureObject)
{
    // first add to texture map
    cout<<"transfering texture to GPU"<<endl;
    textureMap[name] = textureObject;
    unsigned int textureId;
    glGenTextures(1,&textureId);
    glBindTexture(GL_TEXTURE_2D,textureId);
    
    //texture params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Mipmaps are not available for maximization
    
    //copy texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(),textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,textureObject->getImage());
    glGenerateMipmap(GL_TEXTURE_2D);
    
    // worried about race/starvation here. Might need to add a mutex and pass it all the way to pipeline (model->controller->view->pipline)
    // alternative is to run this through the command so that it runs on the main thread. 
    // Hopefully the bottleneck is in file IO and not CPU-GPU memory transfer!

    // running this on the main thread for now.
    //save id in map
    textureIdMap[name] = textureId;
    cout<<"GPU transfer done"<<endl;
}

map<string, unsigned int>& Model::getTextureIdMap()
{
    return this->textureIdMap;
}

map<string, string> Model::getTexturePaths()
{
    return this->texturePaths;
}

void Model::saveTexturePaths(map<string, string> texturePaths)
{
    this->texturePaths = texturePaths;
}

void Model::addNewMesh(string meshName, string meshPath, util::PolygonMesh<VertexAttrib>& polymesh)
{
    scenegraph->addMesh(meshName, meshPath);    
}

void Model::saveCubeMapTextures(vector<util::TextureImage*> cubeMapTextures)
{
    this->cubeMapTextures = cubeMapTextures;
}
void Model::saveCubeMapTexPaths(vector<string> cubeMapTexPaths)
{
    this->cubeMapTexPaths = cubeMapTexPaths;
}

vector<util::TextureImage*>& Model::getCubeMapTextures()
{
    return this->cubeMapTextures;
}
vector<string>& Model::getCubeMapTexPaths()
{
    return this->cubeMapTexPaths;
}