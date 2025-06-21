#include "Model.h"
#include "sgraph/SGNode.h"
#include <GLFW/glfw3.h>
#include <iostream>

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

map<string, util::TextureImage*> Model::getTextureMap()
{
    return this->textureMap;
}

void Model::addToCommandQueue(command::ICommand* command)
{
    cout<<"Adding to command queue"<<endl;
    commandQueue.push(command);
}

void Model::clearCommandQueue()
{
    while(!commandQueue.empty())
    {
        cout<<"model queue is not empty "<<endl;
        command::ICommand* command = commandQueue.front();
        string nodeName = command->getNodeName();
        if(nodes.find(nodeName) != nodes.end())
        {
            // node exists in hierarchy
            cout<<"Found node name"<<endl;
            nodes[nodeName]->accept(command);
        }
        commandQueue.pop();
    }
}


void Model::saveNodes(map<string,sgraph::SGNode *>& nodeMap)
{
    cout<<"Saving nodes"<<endl;
    this->nodes = nodeMap;
}