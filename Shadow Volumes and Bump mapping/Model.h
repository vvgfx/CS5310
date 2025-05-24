#ifndef __MODEL_H__
#define __MODEL_H__

#include "PolygonMesh.h"
#include "VertexAttrib.h"
#include "sgraph/IScenegraph.h"
#include <map>
#include "TextureImage.h"
using namespace std;

class Model 
{
public:
    Model();
    ~Model();
    sgraph::IScenegraph *getScenegraph();
    void setScenegraph(sgraph::IScenegraph *scenegraph);
    void saveTextureMap(map<string, util::TextureImage*>& texMap);
    void saveNormalMap(map<string, util::TextureImage*>& normMap);
    map<string, util::TextureImage*> getTextureMap();
    map<string, util::TextureImage*> getNormalMap();
private:
    
    sgraph::IScenegraph *scenegraph;
    map<string, util::TextureImage*> textureMap;
    map<string, util::TextureImage*> normalMap;

};
#endif