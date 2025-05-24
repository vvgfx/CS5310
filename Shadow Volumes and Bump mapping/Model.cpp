#include "Model.h"
#include <GLFW/glfw3.h>

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

void Model::saveNormalMap(map<string, util::TextureImage*>& normMap)
{
    this->normalMap = normalMap;
}

map<string, util::TextureImage*> Model::getNormalMap()
{
    return this->normalMap;
}