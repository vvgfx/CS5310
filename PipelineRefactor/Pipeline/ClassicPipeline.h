#ifndef _CLASSICPIPELINE_H_
#define _CLASSICPIPELINE_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "IPipeline.h"
#include "../sgraph/IScenegraph.h"
#include <ShaderProgram.h>
#include <ShaderGeoProgram.h>
#include <ShaderLocationsVault.h>
#include "../sgraph/SGNodeVisitor.h"
#include <ObjectInstance.h>
#include <Light.h>
#include "LightLocation.h"
#include <VertexAttrib.h>
#include <TextureImage.h>
#include "../sgraph/GLScenegraphRenderer.h"
#include "../sgraph/LightRetriever.h"


namespace pipeline
{
    /**
     * An implementation the pipeline interface. This pipeline features lights (directional and spotlights) and textures.
     * To use this pipeline, initalize it using init() and draw a single frame using drawFrame()
     */
    class ClassicPipeline : public IPipeline
    {

    public:
        inline void initTextures(map<string, util::TextureImage *> &textureMap);
        inline void computeTangents(util::PolygonMesh<VertexAttrib> &tmesh);
        inline void init(map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage *> texMap, glm::mat4 &projection);
        inline void drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void initLights(sgraph::IScenegraph *scenegraph);

    private:
        util::ShaderProgram shaderProgram;
        util::ShaderLocationsVault shaderLocations;
        sgraph::SGNodeVisitor *renderer;
        sgraph::SGNodeVisitor *lightRetriever;
        map<string, util::ObjectInstance *> objects;
        map<string, unsigned int> textureIdMap;
        vector<util::Light> lights;
        vector<glm::mat4> lightTransformations;
        glm::mat4 projection;
        stack<glm::mat4> modelview;
        std::map<string, sgraph::TransformNode *> cachedNodes;
        vector<LightLocation> lightLocations;
        bool initialized = false;
        int frames;
        double time;
    };

    void ClassicPipeline::init( map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage *> texMap, glm::mat4 &projection)
    {
        this->projection = projection;
        shaderProgram.createProgram("shaders/phong-multiple.vert",
                                    "shaders/phong-multiple.frag");
        shaderProgram.enable();
        shaderLocations = shaderProgram.getAllShaderVariables();
        shaderProgram.disable();

        // Mapping of shader variables to vertex attributes
        map<string, string> shaderVarsToVertexAttribs;
        shaderVarsToVertexAttribs["vPosition"] = "position";
        shaderVarsToVertexAttribs["vNormal"] = "normal";
        shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
        shaderVarsToVertexAttribs["vTangent"] = "tangent";

        for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
             it != meshes.end();
             it++)
        {
            cout << "computing tangents" << endl;
            computeTangents(it->second); // uncomment later
            util::ObjectInstance *obj = new util::ObjectInstance(it->first);
            obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, it->second);
            objects[it->first] = obj;
        }
        initTextures(texMap);
        renderer = new sgraph::GLScenegraphRenderer(modelview, objects, shaderLocations, textureIdMap);
        lightRetriever = new sgraph::LightRetriever(modelview);
        initialized = true;
    }

    void ClassicPipeline::drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        if (!initialized)
            throw runtime_error("pipeline has not been initialized.");

        shaderProgram.enable();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        modelview.push(glm::mat4(1.0f));
        modelview.top() = modelview.top() * viewMat;
        initLights(scenegraph); // lighting scenegraph traversal happens here. I've moved this to the first because the lights need to be initialized
        modelview.pop();

        modelview.push(glm::mat4(1.0));
        modelview.top() = modelview.top() * viewMat;
        glUniform1i(shaderLocations.getLocation("numLights"), lights.size());
        for (int i = 0; i < lights.size(); i++)
        {
            glm::vec4 pos = lights[i].getPosition();
            pos = lightTransformations[i] * pos;
            glm::vec4 spotDirection = lights[i].getSpotDirection();
            spotDirection = lightTransformations[i] * spotDirection;
            // Set light colors
            glUniform3fv(lightLocations[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
            glUniform3fv(lightLocations[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
            glUniform3fv(lightLocations[i].specular, 1, glm::value_ptr(lights[i].getSpecular()));
            glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(pos));
            // spotlight stuff here
            glUniform1f(lightLocations[i].spotAngle, lights[i].getSpotCutoff());
            glUniform3fv(lightLocations[i].spotDirection, 1, glm::value_ptr(spotDirection));
        }

        glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        scenegraph->getRoot()->accept(renderer);

        modelview.pop();
        glFlush();
        shaderProgram.disable();
    }

    void ClassicPipeline::initTextures(map<string, util::TextureImage *> &textureMap)
    {
        for (typename map<string, util::TextureImage *>::iterator it = textureMap.begin(); it != textureMap.end(); it++)
        {
            // first - name of texture, second - texture itself
            util::TextureImage *textureObject = it->second;

            // generate texture ID
            unsigned int textureId;
            glGenTextures(1, &textureId);
            glBindTexture(GL_TEXTURE_2D, textureId);

            // texture params
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Mipmaps are not available for maximization

            // copy texture to GPU
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(), textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, textureObject->getImage());
            glGenerateMipmap(GL_TEXTURE_2D);

            // save id in map
            textureIdMap[it->first] = textureId;
        }
    }

    // Need to move this to a separate class.
    void ClassicPipeline::computeTangents(util::PolygonMesh<VertexAttrib> &tmesh)
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

    void ClassicPipeline::initLights(sgraph::IScenegraph *scenegraph)
    {
        sgraph::LightRetriever *lightsParser = reinterpret_cast<sgraph::LightRetriever *>(lightRetriever);
        lightsParser->clearData();
        scenegraph->getRoot()->accept(lightRetriever);
        lights = lightsParser->getLights();
        lightTransformations = lightsParser->getLightTransformations();
    }

}

#endif