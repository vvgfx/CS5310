#ifndef __ROUNDEDSQUARE_H__
#define __ROUNDEDSQUARE_H__

#include "VertexAttrib.h"
#include <PolygonMesh.h>
#include <GLFW/glfw3.h>

class RoundedSquare: public util::PolygonMesh<VertexAttrib>
{
    public:
    RoundedSquare(int width, int thickness);
    ~RoundedSquare() {}
};

RoundedSquare::RoundedSquare(int width, int thickness)
{
    vector<glm::vec4> positions;

    //add inner square first
    positions.push_back(glm::vec4(-width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, width, 0.0, 1.0));

    positions.push_back(glm::vec4(-width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, width, 0.0, 1.0));
    positions.push_back(glm::vec4(-width, width, 0.0, 1.0));


    //add lower rect
    positions.push_back(glm::vec4(-width, -width-thickness, 0.0, 1.0));
    positions.push_back(glm::vec4(width, -width-thickness, 0.0, 1.0));
    positions.push_back(glm::vec4(width, -width, 0.0, 1.0));

    positions.push_back(glm::vec4(-width, -width-thickness, 0.0, 1.0));
    positions.push_back(glm::vec4(width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(-width, -width, 0.0, 1.0));


    //add right rect
    positions.push_back(glm::vec4(width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(width+thickness, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(width+thickness, width, 0.0, 1.0));

    positions.push_back(glm::vec4(width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(width+thickness, width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, width, 0.0, 1.0));


    //add left rect
    positions.push_back(glm::vec4(-width-thickness, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(-width, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(-width, width, 0.0, 1.0));

    positions.push_back(glm::vec4(-width-thickness, -width, 0.0, 1.0));
    positions.push_back(glm::vec4(-width, width, 0.0, 1.0));
    positions.push_back(glm::vec4(-width-thickness, width, 0.0, 1.0));

    //add top rect
    positions.push_back(glm::vec4(-width, width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, width+thickness, 0.0, 1.0));

    positions.push_back(glm::vec4(-width, width, 0.0, 1.0));
    positions.push_back(glm::vec4(width, width+thickness, 0.0, 1.0));
    positions.push_back(glm::vec4(-width, width+thickness, 0.0, 1.0));

    // add circles after this

    int slices = 30;
    float PI = 3.14159;

    //top right cirle
    glm::vec4 originPoint = glm::vec4(width, width, 0.0, 1.0);
    float oldX = width + thickness, oldY = width;
    for(int i = 0; i < slices; i++)
    {
        float theta = (float)i/(slices - 1) * PI * 0.5;
        float x = width + thickness * cos(theta);
        float y = width + thickness * sin(theta);
        positions.push_back(originPoint);
        positions.push_back(glm::vec4(oldX, oldY, 0.0, 1.0));
        positions.push_back(glm::vec4(x, y, 0.0, 1.0));
        oldX = x;
        oldY = y;
    }

    // top left cirle
    originPoint = glm::vec4(-width, width, 0.0, 1.0);
    oldX = -width;
    oldY = width + thickness;
    for(int i = 0; i < slices; i++)
    {
        float theta = (float)i/(slices - 1) * PI * 0.5 + PI * 0.5;
        float x = -width + thickness * cos(theta);
        float y = width + thickness * sin(theta);
        positions.push_back(originPoint);
        positions.push_back(glm::vec4(oldX, oldY, 0.0, 1.0));
        positions.push_back(glm::vec4(x, y, 0.0, 1.0));
        oldX = x;
        oldY = y;
    }

    // bottom left cirle
    originPoint = glm::vec4(-width, -width, 0.0, 1.0);
    oldX = -width - thickness;
    oldY = -width ;
    for(int i = 0; i < slices; i++)
    {
        float theta = (float)i/(slices - 1) * PI * 0.5 + PI * 1;
        float x = -width + thickness * cos(theta);
        float y = -width + thickness * sin(theta);
        positions.push_back(originPoint);
        positions.push_back(glm::vec4(oldX, oldY, 0.0, 1.0));
        positions.push_back(glm::vec4(x, y, 0.0, 1.0));
        oldX = x;
        oldY = y;
    }

    // bottom right cirle
    originPoint = glm::vec4(width, -width, 0.0, 1.0);
    oldX = width + thickness;
    oldY = -width ;
    for(int i = 0; i < slices; i++)
    {
        float theta = (float)i/(slices - 1) * PI * 0.5 + PI * 1.5;
        float x = width + thickness * cos(theta);
        float y = -width + thickness * sin(theta);
        positions.push_back(originPoint);
        positions.push_back(glm::vec4(oldX, oldY, 0.0, 1.0));
        positions.push_back(glm::vec4(x, y, 0.0, 1.0));
        oldX = x;
        oldY = y;
    }


    // Setting the vertex positions
    vector<VertexAttrib> vertexData;
    for (int i=0;i<positions.size();i++)
    {
        vector<float> data;
        VertexAttrib v;
        for (int j=0;j<4;j++)
            data.push_back(positions[i][j]);
        v.setData("position",data);
        vertexData.push_back(v);
    }

    // Create indices
    vector<unsigned int> indices;
    for (int i=0;i<positions.size();i++)
    {
        indices.push_back(i);
    }

    this->setVertexData(vertexData);
    this->setPrimitives(indices);


    this->setPrimitiveType(GL_TRIANGLES);
    this->setPrimitiveSize(3);

}

#endif
