#include "Model.h"
#include "Rectangle.h"
#include "Circle.h"

Model::Model()
{
    //meshes.push_back(Rectangle(-100,-100,500,200));
    meshes.push_back(Circle(0,0,300));
}

vector<util::PolygonMesh<VertexAttrib> > Model::getMeshes() {
    return meshes;
}

Model::~Model()
{

}

