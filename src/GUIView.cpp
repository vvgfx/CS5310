#include "GUIView.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "sgraph/LightRetriever.h"
#include "sgraph/ShadowRenderer.h"
#include "sgraph/DepthRenderer.h"
#include "sgraph/AmbientRenderer.h"
#include "VertexAttrib.h"
#include "Pipeline/ShadowVolumePipeline.h"
#include "Pipeline/ShadowVolumeBumpMappingPipeline.h"
#include "Pipeline/BasicPBRPipeline.h"
#include "Pipeline/TexturedPBRPipeline.h"
#include "Pipeline/PBRShadowVolumePipeline.h"
#include "Pipeline/TexturedPBRSVPipeline.h"

GUIView::GUIView()
{
}

GUIView::~GUIView()
{
}

void GUIView::init(Callbacks *callbacks, map<string, util::PolygonMesh<VertexAttrib>> &meshes, map<string, util::TextureImage *> texMap)
{
    View::init(callbacks, meshes, texMap);
}

void GUIView::initTextures(map<string, util::TextureImage *> &textureMap)
{
    View::initTextures(textureMap);
}

void GUIView::computeTangents(util::PolygonMesh<VertexAttrib> &tmesh)
{
    View::computeTangents(tmesh);
}

void GUIView::Resize()
{
    View::Resize();
}

void GUIView::updateTrackball(float deltaX, float deltaY)
{
    View::updateTrackball(deltaX, deltaY);
}

void GUIView::resetTrackball()
{
    View::resetTrackball();
}

void GUIView::display(sgraph::IScenegraph *scenegraph)
{
    View::display(scenegraph);
}

void GUIView::initLights(sgraph::IScenegraph *scenegraph)
{
    View::initLights(scenegraph);
}

void GUIView::initLightShaderVars()
{
    View::initLightShaderVars();
}

bool GUIView::shouldWindowClose()
{
    return View::shouldWindowClose();
}

void GUIView::switchShaders()
{
    // not supported
}

void GUIView::closeWindow()
{
    View::closeWindow();
}

// This saves all the nodes required for dynamic transformation
void GUIView::initScenegraphNodes(sgraph::IScenegraph *scenegraph)
{
    View::initScenegraphNodes(scenegraph);
}

void GUIView::rotatePropeller(string nodeName, float time)
{
    // not supported
}

void GUIView::changePropellerSpeed(int num)
{
    // not supported
}

void GUIView::startRotation()
{
    // not supported
}

void GUIView::rotate()
{
    // not supported
}

/**
 * Move/Rotate the drone by passing the matrix to premultiply. This stacks on top of previous input.
 */
void GUIView::moveDrone(int direction)
{
    // not supported
}

/**
 * Set the drone's matrix to what is passed. A good idea would be to pass a rotation and a translation.
 */
void GUIView::setDroneOrientation(glm::mat4 resetMatrix)
{
    // not supported
}

/**
 * Pass a positive yawDir to rotate left, negative to rotate right. Similarly, positive pitchDir to rotate upwards, negative to rotate downwards
 */

void GUIView::rotateDrone(int yawDir, int pitchDir)
{
    // not supported
}

void GUIView::changeCameraType(int cameraType)
{
    // not supported
}