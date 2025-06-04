#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "IPipeline.h"

#include <VertexAttrib.h>
#include <TextureImage.h>
#include <PolygonMesh.h>

namespace pipeline
{
    /**
     * This helper class computes tangents for the provided mesh
     */
    class TangentComputer
    {
        public:
        /**
         * Compute tangents for this mesh.
         */
        static void computeTangents(util::PolygonMesh<VertexAttrib> &tmesh)
        {
            int i, j;
            vector<glm::vec4> tangents;
            vector<float> data;
    
            vector<VertexAttrib> vertexData = tmesh.getVertexAttributes();
            vector<unsigned int> primitives = tmesh.getPrimitives();
            int primitiveSize = tmesh.getPrimitiveSize();
            int vert1, vert2, vert3;
            if (primitiveSize == 6)
            {
                // GL_TRIANGLES_ADJACENCY
                vert1 = 0;
                vert2 = 2;
                vert3 = 4;
            }
            else
            {
                // GL_TRIANGLES
                vert1 = 0;
                vert2 = 1;
                vert3 = 2;
            }
            // initialize as 0
            for (i = 0; i < vertexData.size(); i++)
                tangents.push_back(glm::vec4(0.0f, 0.0, 0.0f, 0.0f));
    
            // go through all the triangles
            for (i = 0; i < primitives.size(); i += primitiveSize)
            {
                // cout<<"i: "<<i<<endl;
                int i0, i1, i2;
                i0 = primitives[i + vert1];
                i1 = primitives[i + vert2];
                i2 = primitives[i + vert3];
    
                // vertex positions
                data = vertexData[i0].getData("position");
                glm::vec3 v0 = glm::vec3(data[0], data[1], data[2]);
    
                data = vertexData[i1].getData("position");
                glm::vec3 v1 = glm::vec3(data[0], data[1], data[2]);
    
                data = vertexData[i2].getData("position");
                glm::vec3 v2 = glm::vec3(data[0], data[1], data[2]);
    
                // UV coordinates
                data = vertexData[i0].getData("texcoord");
                glm::vec2 uv0 = glm::vec2(data[0], data[1]);
    
                data = vertexData[i1].getData("texcoord");
                glm::vec2 uv1 = glm::vec2(data[0], data[1]);
    
                data = vertexData[i2].getData("texcoord");
                glm::vec2 uv2 = glm::vec2(data[0], data[1]);
    
                // Edges of the triangle : position delta
                glm::vec3 deltaPos1 = v1 - v0;
                glm::vec3 deltaPos2 = v2 - v0;
    
                // UV delta
                glm::vec2 deltaUV1 = uv1 - uv0;
                glm::vec2 deltaUV2 = uv2 - uv0;
    
                float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                glm::vec3 tangent = ((deltaPos1 * deltaUV2.y) - (deltaPos2 * deltaUV1.y)) * r;
    
                // change this to support both triangles and triangles adjacency.
                // This accumulates the tangents for each vertex so that the final vertex tangent is smooth.
                tangents[primitives[i + vert1]] = tangents[primitives[i + vert1]] + glm::vec4(tangent, 0.0f);
                tangents[primitives[i + vert2]] = tangents[primitives[i + vert2]] + glm::vec4(tangent, 0.0f);
                tangents[primitives[i + vert3]] = tangents[primitives[i + vert3]] + glm::vec4(tangent, 0.0f);
    
                // for (j = 0; j < 3; j++) {
                //     tangents[primitives[i + j]] =
                //         tangents[primitives[i + j]] + glm::vec4(tangent, 0.0f);
                //     }
                // }
            }
            // orthogonalization
            for (i = 0; i < tangents.size(); i++)
            {
                glm::vec3 t = glm::vec3(tangents[i].x, tangents[i].y, tangents[i].z);
                t = glm::normalize(t);
                data = vertexData[i].getData("normal");
                glm::vec3 n = glm::vec3(data[0], data[1], data[2]);
    
                glm::vec3 b = glm::cross(n, t);
                t = glm::cross(b, n);
    
                t = glm::normalize(t);
    
                tangents[i] = glm::vec4(t, 0.0f);
            }
    
            // set the vertex data
            for (i = 0; i < vertexData.size(); i++)
            {
                data.clear();
                data.push_back(tangents[i].x);
                data.push_back(tangents[i].y);
                data.push_back(tangents[i].z);
                data.push_back(tangents[i].w);
    
                vertexData[i].setData("tangent", data);
            }
            tmesh.setVertexData(vertexData);
        }
    };
}