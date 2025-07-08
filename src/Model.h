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
    void addTexture(string name, string path, util::TextureImage* textureObject);
    // command queue stuff
    void addToCommandQueue(command::ICommand* command);
    void clearQueues();
    void addToTaskQueue(task::ITask* task);

    // save texture
    void saveTexturePaths(map<string, string> texturePaths);
    map<string, string> getTexturePaths();

    // new model at runtime
    void addNewMesh(string meshName, string meshPath, util::PolygonMesh<VertexAttrib>& polymesh);

    //cubemap stuff
    void saveCubeMapTextures(vector<util::TextureImage*>& cubeMapTextures);
    void saveCubeMapTexPaths(map<string, string> cubeMapTexPaths);

    vector<util::TextureImage*>& getCubeMapTextures();
    map<string, string>& getCubeMapTexPaths();

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

    // save textures
    map<string, string> texturePaths;

    // cubemap textures here
    vector<util::TextureImage*> cubeMapTextures;
    map<string, string> cubeMapTexPaths;
};
#endif