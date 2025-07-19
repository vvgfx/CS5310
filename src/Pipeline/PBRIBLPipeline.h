#ifndef _PBRIBLPIPELINE_H_
#define _PBRIBLPIPELINE_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "AbstractPipeline.h"
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
#include "../sgraph/TexturedPBRRenderer.h"
#include "../sgraph/LightRetriever.h"
#include "TangentComputer.h"
#include "stb_image.h"

// temporary imports

namespace pipeline
{
    /**
     * An implementation the pipeline interface. This pipeline features lights (directional and spotlights), textures, PBR workflow and IBL.
     * Note that this pipeline REQUIRES PBR textures to be defined to work properly.
     * To use this pipeline, initalize it using init() and draw a single frame using drawFrame()
     */
    class PBRIBLPipeline : public AbstractPipeline
    {

    public:
        inline void init(map<string, util::PolygonMesh<VertexAttrib>>& meshes, glm::mat4 &projection, map<string, unsigned int>& texMap);
        inline void addMesh(string objectName, util::PolygonMesh<VertexAttrib>& mesh);
        inline void drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat);
        inline void initLights(sgraph::IScenegraph *scenegraph);
        inline void initShaderVars();
        inline void hdrCubemapDraw(glm::mat4 view);
        inline void hdrCubemapInit();

    private:
        util::ShaderProgram shaderProgram;
        util::ShaderProgram hdrSkyboxShaderProgram;
        util::ShaderLocationsVault shaderLocations;
        util::ShaderLocationsVault hdrSkyboxShaderLocations;
        sgraph::SGNodeVisitor *renderer;
        sgraph::SGNodeVisitor *lightRetriever;
        map<string, unsigned int>* textureIdMap;
        vector<util::Light> lights;
        vector<glm::mat4> lightTransformations;
        std::map<string, sgraph::TransformNode *> cachedNodes;
        vector<LightLocation> lightLocations;
        bool initialized = false;
        int frames;
        double time;

        map<string, string> shaderVarsToVertexAttribs;

        // HDR cubemap stuff here
        unsigned int captureFBO, captureRBO, hdrTexture, envCubemap, cubeVAO, cubeVBO;
        int width, height, nrComponents;
    };

    void PBRIBLPipeline::init(map<string, util::PolygonMesh<VertexAttrib>>& meshes, glm::mat4 &proj, map<string, unsigned int>& texMap)
    {
        this->projection = proj;
        shaderProgram.createProgram("shaders/PBR/TexturePBR.vert",
                                    "shaders/PBR/TexturePBR.frag");
        shaderProgram.enable();
        shaderLocations = shaderProgram.getAllShaderVariables();
        shaderProgram.disable();

        hdrSkyboxShaderProgram.createProgram("shaders/cubemap/HDR/background.vert",
                                             "shaders/cubemap/HDR/background.frag");

        hdrSkyboxShaderProgram.enable();
        hdrSkyboxShaderLocations = hdrSkyboxShaderProgram.getAllShaderVariables();
        hdrSkyboxShaderProgram.disable();

        // Mapping of shader variables to vertex attributes
        shaderVarsToVertexAttribs["vPosition"] = "position";
        shaderVarsToVertexAttribs["vNormal"] = "normal";
        shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
        shaderVarsToVertexAttribs["vTangent"] = "tangent";

        textureIdMap = &texMap;

        for (typename map<string, util::PolygonMesh<VertexAttrib>>::iterator it = meshes.begin();
             it != meshes.end();
             it++)
        {
            util::ObjectInstance *obj = new util::ObjectInstance(it->first);
            TangentComputer::computeTangents(it->second); // compute tangents
            obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, it->second);
            objects[it->first] = obj;
        }
        renderer = new sgraph::TexturedPBRRenderer(modelview, objects, shaderLocations, *textureIdMap);
        lightRetriever = new sgraph::LightRetriever(modelview);

        hdrCubemapInit();
        // hdr stuff here

        initialized = true;
    }


    /**
     * Testing method to see if hdr maps work.
     */
    void PBRIBLPipeline::hdrCubemapInit()
    {
        // draw the cubemap and save to memory.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        // generate new framebuffer.
        glGenFramebuffers(1, &captureFBO);
        glGenRenderbuffers(1, &captureRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
        glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 1024, 1024);// format for the render buffer.
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO); // use this render buffer as an attachment.

        util::ShaderProgram equiRectangularShader;
        equiRectangularShader.createProgram("shaders/cubemap/HDR/equiRectangular.vert", 
                                            "shaders/cubemap/HDR/equiRectangular.frag"); // create these!!

        // get shader locations
        equiRectangularShader.enable();
        util::ShaderLocationsVault equiRectShaderLocations = equiRectangularShader.getAllShaderVariables();
        equiRectangularShader.disable();

        // for now, just directly use the imports with stb_image. Will need to rethink this very soon.
        stbi_set_flip_vertically_on_load(true);
        float *data = stbi_loadf("textures/hdr/newport_loft.hdr", &width, &height, &nrComponents, 0);

        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        // set up the cubemap
        glGenTextures(1, &envCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        for(unsigned int i = 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 1024, 1024, 0, GL_RGB, GL_FLOAT, nullptr);
        }
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // set up the views for each face.
        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] =
        {
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
            glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
        };

        equiRectangularShader.enable();
        glUniform1i(equiRectShaderLocations.getLocation("equirectangularMap"), 0); // use TEXTURE0
        glUniformMatrix4fv(equiRectShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(captureProjection));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        glViewport(0, 0, 1024, 1024); // don't forget to configure the viewport to the capture dimensions.
        glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

        for (unsigned int i = 0; i < 6; ++i)
        {
            glUniformMatrix4fv(equiRectShaderLocations.getLocation("view"), 1, GL_FALSE, glm::value_ptr(captureViews[i]));
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, envCubemap, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            objects["hdr-skybox"]->draw();
        }
        equiRectangularShader.disable();

        // reset to original viewport!
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void PBRIBLPipeline::hdrCubemapDraw(glm::mat4 view)
    {
        // initialize the viewmat, then draw the cube with the custom shader!
        // hdrSkyboxShaderProgram.enable();
        // glm::mat4 modelview = glm::scale(view, glm::vec3(10, 10, 10));

        // glUniformMatrix4fv(hdrSkyboxShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        // glUniformMatrix4fv(hdrSkyboxShaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));

        // //skybox sampler2d
        // glActiveTexture(GL_TEXTURE0);
        // unsigned int texID = (*textureIdMap)["hdr-skybox"];
        // glBindTexture(GL_TEXTURE_2D, texID);
        // glUniform1i(hdrSkyboxShaderLocations.getLocation("hdrMap"), 0);

        // objects["box"]->draw();

        // hdrSkyboxShaderProgram.disable();

        hdrSkyboxShaderProgram.enable();
        // backgroundShader.setMat4("view", view);
        glUniformMatrix4fv(hdrSkyboxShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(hdrSkyboxShaderLocations.getLocation("view"), 1, GL_FALSE, glm::value_ptr(view));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, envCubemap);
        glUniform1i(hdrSkyboxShaderLocations.getLocation("environmentMap"), 0);
        objects["hdr-skybox"]->draw();
        hdrSkyboxShaderProgram.disable();

        // cubemapTextureId =  envCubemap;
        // drawCubeMap(view);


    }

    void PBRIBLPipeline::addMesh(string objectName, util::PolygonMesh<VertexAttrib>& mesh)
    {
        TangentComputer::computeTangents(mesh);
        util::ObjectInstance *obj = new util::ObjectInstance(objectName);
        obj->initPolygonMesh(shaderLocations, shaderVarsToVertexAttribs, mesh);
        objects[objectName] = obj;
    }

    void PBRIBLPipeline::drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
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
            spotDirection = lightTransformations[i] * spotDirection;
            // Set light colors
            glUniform3fv(lightLocations[i].color, 1, glm::value_ptr(lights[i].getColor()));
            glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(pos));
            // spotlight stuff here
            glUniform1f(lightLocations[i].spotAngle, cos(glm::radians(lights[i].getSpotCutoff())));
            glUniform3fv(lightLocations[i].spotDirection, 1, glm::value_ptr(spotDirection));
        }

        glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

        scenegraph->getRoot()->accept(renderer);

        // if(cubeMapLoaded)
        //     drawCubeMap(viewMat);

        // test hdr cubemap
        hdrCubemapDraw(viewMat);
        // cout<<"Errors: "<<glGetError()<<endl;
        modelview.pop();
        glFlush();
        shaderProgram.disable();
    }

    void PBRIBLPipeline::initLights(sgraph::IScenegraph *scenegraph)
    {
        sgraph::LightRetriever *lightsParser = reinterpret_cast<sgraph::LightRetriever *>(lightRetriever);
        lightsParser->clearData();
        scenegraph->getRoot()->accept(lightRetriever);
        lights = lightsParser->getLights();
        lightTransformations = lightsParser->getLightTransformations();
    }

    void PBRIBLPipeline::initShaderVars()
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
            ll.spotAngle = shaderLocations.getLocation(name.str() + ".spotAngleCosine");
            lightLocations.push_back(ll);
        }
    }
}

#endif