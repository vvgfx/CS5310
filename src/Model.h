#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
#include "TextureImage.h"
#include "sgraph/Commands/ICommand.h"
#include "Tasks/ITask.h"
#include <queue>
#include <mutex>
using namespace std;

class Model 
{
public:
    Model();
    ~Model();
    sgraph::IScenegraph *getScenegraph();
    void setScenegraph(sgraph::IScenegraph *scenegraph);
    void saveTextureMap(map<string, util::TextureImage*> texMap);
    map<string, util::TextureImage*>& getTextureMap();
    map<string, unsigned int>& getTextureIdMap();
    void initTextures(map<string, util::TextureImage*>& textureMap);
    void addTexture(string name, util::TextureImage* textureObject);
    // command queue stuff
    void addToCommandQueue(command::ICommand* command);
    void clearQueues();
    void addToTaskQueue(task::ITask* task);

private:
    
    sgraph::IScenegraph *scenegraph;
    map<string, util::TextureImage*> textureMap;
    map<string, unsigned int> textureIdMap;
    queue<command::ICommand*> frontCommandQueue;
    queue<command::ICommand*> backCommandQueue;
    queue<task::ITask*> frontTaskQueue;
    queue<task::ITask*> backTaskQueue;
    mutex commandMutex;
    mutex taskMutex;
};
#endif