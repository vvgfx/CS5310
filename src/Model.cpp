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

void Model::saveTextureMap(map<string, util::TextureImage*>& texMap)
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
    m.lock();
    backQueue.push(command);
    m.unlock();
}

void Model::clearCommandQueue()
{
    if(backQueue.empty())
        return;
    m.lock();
    std::swap(backQueue, frontQueue);
    m.unlock();
    while(!frontQueue.empty())
    {
        cout<<"model queue is not empty : "<<frontQueue.size()<<endl;
        command::ICommand* command = frontQueue.front();
        string nodeName = command->getNodeName();
        map<string, sgraph::SGNode *>* nodes = scenegraph->getNodes();
        if(nodes->find(nodeName) != nodes->end())
        {
            // node exists in hierarchy
            (*nodes)[nodeName]->accept(command);
        }
        frontQueue.pop();
    }
}

