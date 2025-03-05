#include "Model.h"
#include "CircleLine.h"
// #include "RoundedSquare.h"

Model::Model()
{
    meshes.push_back(CircleLine(0,0,400));
}

vector<util::PolygonMesh<VertexAttrib> > Model::getMeshes() {
    return meshes;
}

Model::~Model()
{

}
