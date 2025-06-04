#ifndef _SHADOWVOLUMEPIPELINE_H_
#define _SHADOWVOLUMEPIPELINE_H_

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
#include "../sgraph/DepthRenderer.h"
#include "../sgraph/AmbientRenderer.h"
#include "../sgraph/ShadowRenderer.h"
#include "../sgraph/LightRetriever.h"

namespace pipeline
{
    /**
     * An implementation the pipeline interface. This pipeline features lights (directional and spotlights), textures and shadow volumes.
     * To use this pipeline, initalize it using init() and draw a single frame using drawFrame()
     */
    class ShadowVolumePipeline : public IPipeline
    {

    public:
        inline void initTextures(map<string, util::TextureImage *> &textureMap);
        inline void computeTangents(util::PolygonMesh<VertexAttrib> &tmesh);
        inline void init(map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage *> &texMap, glm::mat4 &projection);
        inline void drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void initLights(sgraph::IScenegraph *scenegraph);
        inline void initLightShaderVars();
        inline void depthPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void shadowStencilPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex);
        inline void renderObjectPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex);
        inline void ambientPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);

    private:
        util::ShaderProgram renderProgram;
        util::ShaderGeoProgram shadowProgram;
        util::ShaderProgram depthProgram;
        util::ShaderProgram ambientProgram;
        util::ShaderLocationsVault renderShaderLocations;
        util::ShaderLocationsVault shadhowShaderLocations;
        util::ShaderLocationsVault depthShaderLocations;
        util::ShaderLocationsVault ambientShaderLocations;
        sgraph::SGNodeVisitor *renderer;
        sgraph::SGNodeVisitor *lightRetriever;
        sgraph::SGNodeVisitor *shadowRenderer;
        sgraph::SGNodeVisitor *depthRenderer;
        sgraph::SGNodeVisitor *ambientRenderer;
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

    void ShadowVolumePipeline::init(map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage *> &texMap, glm::mat4 &proj)
    {
        this->projection = proj;

        renderProgram.createProgram(string("shaders/shadow/phong-shadow.vert"),
                                    string("shaders/shadow/phong-shadow.frag"));
        renderProgram.enable();
        renderShaderLocations = renderProgram.getAllShaderVariables();
        renderProgram.disable();

        // depth program for shadow first pass
        depthProgram.createProgram(string("shaders/shadow/depth.vert"),
                                   string("shaders/shadow/depth.frag"));
        depthProgram.enable();
        depthShaderLocations = depthProgram.getAllShaderVariables();
        depthProgram.disable();

        // ambient program for shadow final pass
        ambientProgram.createProgram(string("shaders/shadow/ambient.vert"),
                                     string("shaders/shadow/ambient.frag"));
        ambientProgram.enable();
        ambientShaderLocations = ambientProgram.getAllShaderVariables();
        ambientProgram.disable();

        // shadow shaders next
        shadowProgram.createProgram(string("shaders/shadow/shadow.vert"),
                                    string("shaders/shadow/shadow.frag"),
                                    string("shaders/shadow/shadow.geom"));
        shadowProgram.enable();
        shadhowShaderLocations = shadowProgram.getAllShaderVariables();
        shadowProgram.disable();

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
            util::ObjectInstance *obj = new util::ObjectInstance(it->first);
            obj->initPolygonMesh(renderShaderLocations, shaderVarsToVertexAttribs, it->second);
            objects[it->first] = obj;
        }
        initTextures(texMap);
        renderer = new sgraph::GLScenegraphRenderer(modelview, objects, renderShaderLocations, textureIdMap);
        lightRetriever = new sgraph::LightRetriever(modelview);
        shadowRenderer = new sgraph::ShadowRenderer(modelview, objects, shadhowShaderLocations);

        // scenegraph renderers for shadow volumes
        depthRenderer = new sgraph::DepthRenderer(modelview, objects, depthShaderLocations);
        ambientRenderer = new sgraph::AmbientRenderer(modelview, objects, ambientShaderLocations);

        initialized = true;
    }

    void ShadowVolumePipeline::drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        if (!initialized)
            throw runtime_error("pipeline has not been initialized.");

        modelview.push(glm::mat4(1.0f));
        modelview.top() = modelview.top() * viewMat;
        initLights(scenegraph); // lighting scenegraph traversal happens here. I've moved this to the first because the lights need to be initialized
        modelview.pop();
        // initShaderVars(); -> dont require this??
        
        #pragma region drawFrame
        glDepthFunc(GL_LEQUAL);
        glClearColor(0, 0, 0, 1);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);                                                       // enable writing to the depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear everything before starting the render loop.

        depthPass(scenegraph, viewMat); // set the depth buffer from the actual camera location to set up for the stencil test.
        glEnable(GL_STENCIL_TEST);      // enable stencil test.
        glEnable(GL_BLEND);             // for multiple lights
        glBlendFunc(GL_ONE, GL_ONE);    //  Equally blend all the effects from eall the lights (is this correct?)
        for (int i = 0; i < lights.size(); i++)
        {
            glClear(GL_STENCIL_BUFFER_BIT);
            shadowStencilPass(scenegraph, viewMat, i); // render the shadow volume into the stencil buffer.
            renderObjectPass(scenegraph, viewMat, i);  // render all the objects with lighting (except ambient) into the scene. (fragments that fail the stencil test will not touch the fragment shader).
        }
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);       // need to disable the stencil test for the ambient pass because all objects require ambient lighting.
        ambientPass(scenegraph, viewMat); // ambient pass for all objects.

        #pragma endregion
    }

    void ShadowVolumePipeline::depthPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        glDrawBuffer(GL_NONE); // Don't want to draw anything, only fill the depth buffer.
        depthProgram.enable();
        modelview.push(glm::mat4(1.0));
        modelview.top() = modelview.top() * viewMat;
        glUniformMatrix4fv(depthShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // the modelview will be passed by the renderer (hopefully)
        scenegraph->getRoot()->accept(depthRenderer);
        modelview.pop();
        depthProgram.disable();
    }

    void ShadowVolumePipeline::shadowStencilPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex)
    {
        glDepthMask(GL_FALSE);             // do not write into the depth buffer anymore. This is so that the shadow volumes do not obstruct the actual objects.
        glEnable(GL_DEPTH_CLAMP);          // Don't want to clip the back polygons.
        glDisable(GL_CULL_FACE);           // Don't want the back-facing polygons to get culled. need them to increment the stencil buffer.
        glStencilFunc(GL_ALWAYS, 0, 0xff); // Always pass the stencil test, reference value 0,mask value 1(should probably use ~0)

        // For some reason, the depth-fail method breaks down for some fragments of the sphere.
        // depth fail
        // glStencilOpSeparate(GL_BACK, // for backfacing polygons
        //                     GL_KEEP, // stencil test fails - doesnt happen because stencil function is set to always pass
        //                     GL_INCR_WRAP, // stencil passes but depth fails - our required condition - increment the stencil buffer value
        //                     GL_KEEP); // both stencil and depth passes - not relevant; do nothing.

        // glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP); // similarly, for front facing polygons, decrement the stencil buffer

        // These are for depth pass. This works for now, but will fail when the camera is placed inside a shadow.
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

        // I think I can swap the incrememnt and decrement and I'll still be fine.

        // Now render the scene
        shadowProgram.enable();
        modelview.push(glm::mat4(1.0));
        modelview.top() = modelview.top() * viewMat;
        glUniformMatrix4fv(shadhowShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // did the light traversal at the first now.
        glm::vec4 pos = lights[lightIndex].getPosition();
        pos = lightTransformations[lightIndex] * pos;
        // remember that all the lightlocations are in the view coordinate system.
        glm::vec3 sendingVal = glm::vec3(pos);
        glUniform3fv(shadhowShaderLocations.getLocation("gLightPos"), 1, glm::value_ptr(sendingVal));
        scenegraph->getRoot()->accept(shadowRenderer);

        modelview.pop();
        shadowProgram.disable();

        // restore original settings now
        glDisable(GL_DEPTH_CLAMP);
        glEnable(GL_CULL_FACE); // need to enable culling so that the next pass doesn't render all faces.
    }

    void ShadowVolumePipeline::renderObjectPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex)
    {

        renderProgram.enable();
        glDrawBuffer(GL_BACK);                                   // Enable writing to the color buffer. This was disabled in the depth pass.
        glStencilFunc(GL_EQUAL, 0x0, 0xFF);                      // draw only if stencil value is 0
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP); // do not write to the stencil buffer.

        // glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); // clearing all the buffers in the main method now.

        // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT_FACE);

        modelview.push(glm::mat4(1.0));
        modelview.top() = modelview.top() * viewMat; // this should return the same behavior now.
        // lighting setup now happens at display(). This is because the lights are required for the stencil shadow pass
        //  which runs before the renderObject pass.

        glUniform1i(renderShaderLocations.getLocation("numLights"), lights.size());

        // send the data for the ith light
        glm::vec4 pos = lights[lightIndex].getPosition();
        pos = lightTransformations[lightIndex] * pos;
        // position
        // adding direction for spotlight
        glm::vec4 spotDirection = lights[lightIndex].getSpotDirection();
        spotDirection = lightTransformations[lightIndex] * spotDirection;
        // Set light colors
        glUniform3fv(renderShaderLocations.getLocation("light.ambient"), 1, glm::value_ptr(lights[lightIndex].getAmbient()));
        glUniform3fv(renderShaderLocations.getLocation("light.diffuse"), 1, glm::value_ptr(lights[lightIndex].getDiffuse()));
        glUniform3fv(renderShaderLocations.getLocation("light.specular"), 1, glm::value_ptr(lights[lightIndex].getSpecular()));
        glUniform4fv(renderShaderLocations.getLocation("light.position"), 1, glm::value_ptr(pos));
        // spotlight stuff here
        glUniform1f(renderShaderLocations.getLocation("light.spotAngle"), lights[lightIndex].getSpotCutoff());
        glUniform3fv(renderShaderLocations.getLocation("light.spotDirection"), 1, glm::value_ptr(spotDirection));

        // send projection matrix to GPU
        glUniformMatrix4fv(renderShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // draw scene graph here
        scenegraph->getRoot()->accept(renderer);

        // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
        // glEnable(GL_CULL_FACE);
        // glCullFace(GL_FRONT_FACE);
        modelview.pop();
        renderProgram.disable();
    }

    void ShadowVolumePipeline::ambientPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        ambientProgram.enable();
        glEnable(GL_BLEND);           // blend the ambient light.
        glBlendEquation(GL_FUNC_ADD); // blend by addition.
        glBlendFunc(GL_ONE, GL_ONE);  // equal parts of existing and ambient. This is fine because the ambient shader has intensity reduced to 0.2 times.
        modelview.push(glm::mat4(1.0));
        modelview.top() = modelview.top() * viewMat;
        glUniformMatrix4fv(ambientShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // This uses only the material's ambient. Ideally it should use each light's ambient as well. Maybe later tho.
        scenegraph->getRoot()->accept(ambientRenderer);
        modelview.pop();
        ambientProgram.disable();
        glDisable(GL_BLEND);
    }

    void ShadowVolumePipeline::initTextures(map<string, util::TextureImage *> &textureMap)
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
    void ShadowVolumePipeline::computeTangents(util::PolygonMesh<VertexAttrib> &tmesh)
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

    void ShadowVolumePipeline::initLights(sgraph::IScenegraph *scenegraph)
    {
        sgraph::LightRetriever *lightsParser = reinterpret_cast<sgraph::LightRetriever *>(lightRetriever);
        lightsParser->clearData();
        scenegraph->getRoot()->accept(lightRetriever);
        lights = lightsParser->getLights();
        lightTransformations = lightsParser->getLightTransformations();
    }

    void ShadowVolumePipeline::initLightShaderVars()
    {
        lightLocations.clear();
        for (int i = 0; i < lights.size(); i++)
        {
            LightLocation ll;
            stringstream name;

            name << "light[" << i << "]";
            ll.ambient = renderShaderLocations.getLocation(name.str() + "" + ".ambient");
            ll.diffuse = renderShaderLocations.getLocation(name.str() + ".diffuse");
            ll.specular = renderShaderLocations.getLocation(name.str() + ".specular");
            ll.position = renderShaderLocations.getLocation(name.str() + ".position");
            // adding spotDirection and spotAngle.
            ll.spotDirection = renderShaderLocations.getLocation(name.str() + ".spotDirection");
            ll.spotAngle = renderShaderLocations.getLocation(name.str() + ".spotAngle");
            lightLocations.push_back(ll);
        }
    }

}

#endif