#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
#include "TextureImage.h"
#include "sgraph/Command/ICommand.h"
#include <queue>
using namespace std;

class Model 
{
public:
    Model();
    ~Model();
    sgraph::IScenegraph *getScenegraph();
    void setScenegraph(sgraph::IScenegraph *scenegraph);
    void saveTextureMap(map<string, util::TextureImage*>& texMap);
    map<string, util::TextureImage*> getTextureMap();
    
    // command queue stuff
    void saveNodes(map<string,sgraph::SGNode *>& nodes);
    void addToCommandQueue(command::ICommand* command);
    void clearCommandQueue();

private:
    
    sgraph::IScenegraph *scenegraph;
    map<string,sgraph::SGNode *> nodes;
    map<string, util::TextureImage*> textureMap;

    queue<command::ICommand*> commandQueue;

};
#endif