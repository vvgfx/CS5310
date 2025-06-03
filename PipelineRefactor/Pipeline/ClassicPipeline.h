#ifndef _CLASSICPIPELINE_H_
#define _CLASSICPIPELINE_H_

#include "IPipeline.h"
#include "glm/glm.hpp"
#include "sgraph/IScenegraph.h"
#include <ShaderProgram.h>
#include <ShaderGeoProgram.h>
#include <ShaderLocationsVault.h>
#include "sgraph/SGNodeVisitor.h"
#include <ObjectInstance.h>
#include <Light.h>
#include "Pipeline/LightLocation.h"
#include <VertexAttrib.h>
#include <TextureImage.h>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/LightRetriever.h"

namespace pipeline
{
    /**
     * An implementation the pipeline interface. This pipeline features lights (directional and spotlights) and textures.
     * To use this pipeline, initalize it using init() and draw a single frame using drawFrame()
     */
    class ClassicPipeline : public IPipeline
    {
    public:
        void initTextures(map<string, util::TextureImage*> texMap);
        void init(glm::mat4 &mv, map<string,util::PolygonMesh<VertexAttrib>>& meshes, map<string, util::TextureImage*> texMap, glm::mat4& projection)
        {
            
        }

        void drawFrame(sgraph::IScenegraph *scenegraph, glm::mat4 &viewMat)
        {
        }

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
    };


    void ClassicPipeline::initTextures(map<string, util::TextureImage*> texMap)
    {

    }
}

#endif