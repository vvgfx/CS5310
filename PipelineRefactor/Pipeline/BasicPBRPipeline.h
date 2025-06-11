#ifndef _BASICPBRPIPELINE_H_
#define _BASICPBRPIPELINE_H_

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
#include "../sgraph/BasicPBRRenderer.h"
#include "../sgraph/LightRetriever.h"
#include "TangentComputer.h"
#include <iostream>

namespace pipeline
{
    /**
     * An implementation the pipeline interface. This pipeline features lights (directional and spotlights) and PBR workflow.
     * Note that this pipeline REQUIRES PBR materials to be defined to work properly, and does NOT support ANY texture.
     * To use this pipeline, initalize it using init() and draw a single frame using drawFrame()
     * 
     * Note: This pipeline does all the PBR calculations in the world space.
     */
    class BasicPBRPipeline : public IPipeline
    {

    public:
        inline void init(map<string, util::PolygonMesh<VertexAttrib>>& meshes, glm::mat4 &projection);
        inline void drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void initLights(sgraph::IScenegraph *scenegraph);
        inline void initShaderVars();

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
        glm::vec3 cameraPos;
        bool initialized = false;
        int frames;
        double time;
    };

    void BasicPBRPipeline::init(map<string, util::PolygonMesh<VertexAttrib>>& meshes, glm::mat4 &proj)
    {
        this->projection = proj;
        shaderProgram.createProgram("shaders/PBR/PBR.vert",
                                    "shaders/PBR/PBR.frag");
        shaderProgram.enable();
        shaderLocations = shaderProgram.getAllShaderVariables();
        shaderProgram.disable();

        // Mapping of shader variables to vertex attributes
        map<string, string> shaderVarsToVertexAttribs;
        shaderVarsToVertexAttribs["vPosition"] = "position";
        shaderVarsToVertexAttribs["vNormal"] = "normal";

        for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
             it != meshes.end();
             it++)
        {
            util::ObjectInstance *obj = new util::ObjectInstance(it->first);
            obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, it->second);
            objects[it->first] = obj;
        }
        renderer = new sgraph::BasicPBRRenderer(modelview, objects, shaderLocations);
        lightRetriever = new sgraph::LightRetriever(modelview);
        initialized = true;
    }

    void BasicPBRPipeline::drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
    {
        if (!initialized)
            throw runtime_error("pipeline has not been initialized.");

        // can't pass the view vec3 directly so doing this.
        glm::mat4 inverseView = glm::inverse(viewMat);
        cameraPos = glm::vec3(inverseView[3]);

        shaderProgram.enable();
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        modelview.push(glm::mat4(1.0f));
        // modelview.top() = modelview.top() * viewMat; // This means all the lights will be in the view coordinate system. - Commented out because all transformations are in the world coordinate system now
        initLights(scenegraph); // lighting scenegraph traversal happens here. I've moved this to the first because the lights need to be initialized
        initShaderVars();
        modelview.pop();

        // passing the camera location to the fragment shader.
        glUniform3fv(shaderLocations.getLocation("cameraPos"), 1, glm::value_ptr(cameraPos));
        glUniformMatrix4fv(shaderLocations.getLocation("view"), 1, GL_FALSE, glm::value_ptr(viewMat)); // view transformation

        modelview.push(glm::mat4(1.0));
        // modelview.top() = modelview.top() * viewMat; // -> needs to be in the world coordinate system!
        glUniform1i(shaderLocations.getLocation("numLights"), lights.size());
        for (int i = 0; i < lights.size(); i++)
        {
            glm::vec4 pos = lights[i].getPosition();
            pos = lightTransformations[i] * pos; // world coordinate system.
            glm::vec4 spotDirection = lights[i].getSpotDirection();
            spotDirection = lightTransformations[i] * spotDirection; // world coordinate system.
            // Set light colors
            glUniform3fv(lightLocations[i].color, 1, glm::value_ptr(lights[i].getColor()));
            glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(pos));
            // spotlight stuff here
            glUniform1f(lightLocations[i].spotAngle, cos(glm::radians(lights[i].getSpotCutoff())));
            glUniform3fv(lightLocations[i].spotDirection, 1, glm::value_ptr(spotDirection));
        }

        glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        scenegraph->getRoot()->accept(renderer);
        // cout<<"Errors: "<<glGetError()<<endl;
        modelview.pop();
        glFlush();
        shaderProgram.disable();
    }

    void BasicPBRPipeline::initLights(sgraph::IScenegraph *scenegraph)
    {
        sgraph::LightRetriever *lightsParser = reinterpret_cast<sgraph::LightRetriever *>(lightRetriever);
        lightsParser->clearData();
        scenegraph->getRoot()->accept(lightRetriever);
        lights = lightsParser->getLights();
        lightTransformations = lightsParser->getLightTransformations();
    }

    void BasicPBRPipeline::initShaderVars()
    {
        lightLocations.clear();
        for (int i = 0; i < lights.size(); i++)
        {
            LightLocation ll;
            stringstream name;

            name << "light[" << i << "]";
            ll.position = shaderLocations.getLocation(name.str() + "" + ".position");
            ll.color = shaderLocations.getLocation(name.str() + ".color");
            // adding spotDirection and spotAngle.
            ll.spotDirection = shaderLocations.getLocation(name.str() + ".spotDirection");
            // cout<<"spot direction location: "<<ll.spotDirection<<endl;
            ll.spotAngle = shaderLocations.getLocation(name.str() + ".spotAngleCosine");
            lightLocations.push_back(ll);
        }
    }

}

#endif