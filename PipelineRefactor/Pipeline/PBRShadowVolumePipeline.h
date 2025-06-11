#ifndef _PBRSHADOWVOLUMEPIPELINE_H_
#define _PBRSHADOWVOLUMEPIPELINE_H_

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
#include "../sgraph/PBRShadowVolRenderer.h"
#include "../sgraph/LightRetriever.h"
#include "../sgraph/ShadowRenderer.h"
#include "../sgraph/DepthRenderer.h"
#include "../sgraph/PBRAmbientRenderer.h"
#include "TangentComputer.h"
#include <iostream>

namespace pipeline
{
    /**
     * An implementation the pipeline interface. This pipeline features lights (directional and spotlights), shadows using shadow volumes and PBR workflow.
     * Note that this pipeline REQUIRES PBR materials to be defined to work properly, and does NOT support ANY texture.
     * To use this pipeline, initalize it using init() and draw a single frame using drawFrame()
     */
    class PBRShadowVolumePipeline : public IPipeline
    {

    public:
        inline void init(map<string, util::PolygonMesh<VertexAttrib>> &meshes, glm::mat4 &projection);
        inline void drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void initLights(sgraph::IScenegraph *scenegraph);
        inline void depthPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void shadowStencilPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex);
        inline void renderObjectPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex);
        inline void ambientPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);

    private:
        util::ShaderProgram renderProgram;
        util::ShaderProgram depthProgram;
        util::ShaderProgram ambientProgram;
        util::ShaderGeoProgram shadowProgram;

        util::ShaderLocationsVault renderShaderLocations;
        util::ShaderLocationsVault depthShaderLocations;
        util::ShaderLocationsVault ambientShaderLocations;
        util::ShaderLocationsVault shadowShaderLocations;

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
        glm::vec3 cameraPos;
        bool initialized = false;
        int frames;
        double time;
    };

    void PBRShadowVolumePipeline::init(map<string, util::PolygonMesh<VertexAttrib>> &meshes, glm::mat4 &proj)
    {
        this->projection = proj;

        // Render program initialization
        renderProgram.createProgram("shaders/shadow/PBR-SV.vert",
                                    "shaders/shadow/PBR-SV.frag");
        renderProgram.enable();
        renderShaderLocations = renderProgram.getAllShaderVariables();
        renderProgram.disable();

        // Depth program initialization
        depthProgram.createProgram("shaders/shadow/depth.vert",
                                   "shaders/shadow/depth.frag");
        depthProgram.enable();
        depthShaderLocations = depthProgram.getAllShaderVariables();
        depthProgram.disable();

        // Ambient program initialization
        ambientProgram.createProgram("shaders/shadow/PBR-ambient.vert",
                                     "shaders/shadow/PBR-ambient.frag");
        ambientProgram.enable();
        ambientShaderLocations = ambientProgram.getAllShaderVariables();
        ambientProgram.disable();

        // Shadow program initialization
        shadowProgram.createProgram("shaders/shadow/shadow.vert",
                                    "shaders/shadow/shadow.frag",
                                    "shaders/shadow/shadow.geom");
        shadowProgram.enable();
        shadowShaderLocations = shadowProgram.getAllShaderVariables();
        shadowProgram.disable();

        // Mapping of shader variables to vertex attributes
        map<string, string> shaderVarsToVertexAttribs;
        shaderVarsToVertexAttribs["vPosition"] = "position";
        shaderVarsToVertexAttribs["vNormal"] = "normal";

        for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
             it != meshes.end();
             it++)
        {
            util::ObjectInstance *obj = new util::ObjectInstance(it->first);
            obj->initPolygonMesh(renderShaderLocations, shaderVarsToVertexAttribs, it->second);
            objects[it->first] = obj;
        }

        renderer = new sgraph::PBRShadowVolRenderer(modelview, objects, renderShaderLocations);
        lightRetriever = new sgraph::LightRetriever(modelview);
        shadowRenderer = new sgraph::ShadowRenderer(modelview, objects, shadowShaderLocations);
        depthRenderer = new sgraph::DepthRenderer(modelview, objects, depthShaderLocations);
        ambientRenderer = new sgraph::PBRAmbientRenderer(modelview, objects, ambientShaderLocations);
        initialized = true;
    }

    void PBRShadowVolumePipeline::drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        if (!initialized)
            throw runtime_error("pipeline has not been initialized.");

        // Light traversal
        modelview.push(glm::mat4(1.0f));
        modelview.top() = modelview.top() * viewMat;
        initLights(scenegraph); // lighting scenegraph traversal happens here.
        modelview.pop();
        glDepthFunc(GL_LEQUAL);

        // shadow volume pipeline starts here.
        glClearColor(0, 0, 0, 1);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);                                                       // enable writing to the depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear everything before starting the render loop.

        depthPass(scenegraph, viewMat); // set the depth buffer from the actual camera location to set up for the stencil test.

        glEnable(GL_STENCIL_TEST);   // enable stencil test.
        glEnable(GL_BLEND);          // for multiple lights
        glBlendFunc(GL_ONE, GL_ONE); //  Equally blend all the effects from eall the lights (is this correct?)
        for (int i = 0; i < lights.size(); i++)
        {
            glClear(GL_STENCIL_BUFFER_BIT);
            shadowStencilPass(scenegraph, viewMat, i); // render the shadow volume into the stencil buffer.
            renderObjectPass(scenegraph, viewMat, i);  // render all the objects with lighting (except ambient) into the scene. (fragments that fail the stencil test will not touch the fragment shader).
        }
        glDisable(GL_BLEND);
        glDisable(GL_STENCIL_TEST);       // need to disable the stencil test for the ambient pass because all objects require ambient lighting.
        ambientPass(scenegraph, viewMat); // ambient pass for all objects.
        // Note to self: In order to do postprocessing, I might need to write the output to a different framebuffer and then read that as a texture to my post-processing pass
    }

    void PBRShadowVolumePipeline::initLights(sgraph::IScenegraph *scenegraph)
    {
        sgraph::LightRetriever *lightsParser = reinterpret_cast<sgraph::LightRetriever *>(lightRetriever);
        lightsParser->clearData();
        scenegraph->getRoot()->accept(lightRetriever);
        lights = lightsParser->getLights();
        lightTransformations = lightsParser->getLightTransformations();
    }

    void PBRShadowVolumePipeline::depthPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
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

    void PBRShadowVolumePipeline::shadowStencilPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex)
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
        glUniformMatrix4fv(shadowShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // did the light traversal at the first now.
        glm::vec4 pos = lights[lightIndex].getPosition();
        pos = lightTransformations[lightIndex] * pos;
        // remember that all the lightlocations are in the view coordinate system.
        glm::vec3 sendingVal = glm::vec3(pos);
        glUniform3fv(shadowShaderLocations.getLocation("gLightPos"), 1, glm::value_ptr(sendingVal));
        scenegraph->getRoot()->accept(shadowRenderer);

        modelview.pop();
        shadowProgram.disable();

        // restore original settings now
        glDisable(GL_DEPTH_CLAMP);
        glEnable(GL_CULL_FACE); // need to enable culling so that the next pass doesn't render all faces.
    }

    void PBRShadowVolumePipeline::ambientPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        // TODO: Pass the material's albedo and ao in PBRAmbientRenderer.
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

    void PBRShadowVolumePipeline::renderObjectPass(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat, int lightIndex)
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
        // TODO : Update this code for PBR instead of phong.
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
}

#endif