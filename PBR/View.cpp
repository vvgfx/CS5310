#include "View.h"
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
#include "sgraph/PBRRenderer.h"
#include "VertexAttrib.h"


View::View() {

}

View::~View(){

}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes, map<string, util::TextureImage*> texMap)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_STENCIL_BITS, 8);  // enable stencil buffer
    glfwWindowHint(GLFW_DEPTH_BITS, 24);   // For depth testing
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);  // Double buffering

    window = glfwCreateWindow(1280, 720, "Shadow Volumes and Bump Mapping", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
     glfwSetWindowUserPointer(window, (void *)callbacks);

    //using C++ functions as callbacks to a C-style library
    glfwSetKeyCallback(window, 
    [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onkey(key,scancode,action,mods);
    });

    glfwSetWindowSizeCallback(window, 
    [](GLFWwindow* window, int width,int height)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->reshape(width,height);
    });

    glfwSetMouseButtonCallback(window, 
    [](GLFWwindow* window, int button, int action, int mods)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onMouseInput(button, action, mods);
    });

    glfwSetCursorPosCallback(window, 
    [](GLFWwindow* window, double xpos, double ypos)
    {
        reinterpret_cast<Callbacks*>(glfwGetWindowUserPointer(window))->onCursorMove(xpos, ypos);
    });

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // create the shader programs

    // render shaders first
    renderProgram.createProgram(string("shaders/PBR/PBR.vert"),
                                string("shaders/PBR/PBR.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    renderProgram.enable();
    renderShaderLocations = renderProgram.getAllShaderVariables();
    renderProgram.disable();

    //depth program for shadow first pass
    depthProgram.createProgram(string("shaders/shadow/depth.vert"),
                                string("shaders/shadow/depth.frag"));
    depthProgram.enable();
    depthShaderLocations = depthProgram.getAllShaderVariables();
    depthProgram.disable();

    //ambient program for shadow final pass
    ambientProgram.createProgram(string("shaders/shadow/ambient.vert"),
                                string("shaders/shadow/ambient.frag"));
    ambientProgram.enable();
    ambientShaderLocations = ambientProgram.getAllShaderVariables();
    ambientProgram.disable();


    //shadow shaders next
    shadowProgram.createProgram(string("shaders/shadow/shadow.vert"),
                                string("shaders/shadow/shadow.frag"),
                                string("shaders/shadow/shadow.geom"));
    shadowProgram.enable();
    shadhowShaderLocations = shadowProgram.getAllShaderVariables();
    shadowProgram.disable();
    

    
    /* In the mesh, we have some attributes for each vertex. In the shader
     * we have variables for each vertex attribute. We have to provide a mapping
     * between attribute name in the mesh and corresponding shader variable
     name.
     *
     * This will allow us to use PolygonMesh with any shader program, without
     * assuming that the attribute names in the mesh and the names of
     * shader variables will be the same.

       We create such a shader variable -> vertex attribute mapping now
     */
    map<string, string> shaderVarsToVertexAttribs;
    // This is a singular mapping for all the shader programs. Might have issues with variable names later.
    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
    shaderVarsToVertexAttribs["vTangent"] = "tangent"; //adding tangent data for bump mapping. - uncomment later.
    
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) 
    {
        cout<<"computing tangents"<<endl;
        computeTangents(it->second); // uncomment later
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(renderShaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    cout<<"computed tangents!"<<endl;
	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    frames = 0;
    time = glfwGetTime();

    initTextures(texMap);
    renderer = new sgraph::PBRRenderer(modelview, objects, renderShaderLocations, textureIdMap);
    lightRetriever = new sgraph::LightRetriever(modelview);
    shadowRenderer = new sgraph::ShadowRenderer(modelview, objects, shadhowShaderLocations);

    //scenegraph renderers for shadow volumes
    depthRenderer = new sgraph::DepthRenderer(modelview, objects, depthShaderLocations);
    ambientRenderer = new sgraph::AmbientRenderer(modelview, objects, ambientShaderLocations);
    // cout<<"Error:"<<glGetError()<<endl;
}

void View::initTextures(map<string, util::TextureImage*>& textureMap)
{
    for(typename map<string, util::TextureImage*>::iterator it = textureMap.begin(); it!=textureMap.end(); it++)
    {
        //first - name of texture, second - texture itself
        util::TextureImage* textureObject = it->second;
        
        //generate texture ID
        unsigned int textureId;
        glGenTextures(1,&textureId);
        glBindTexture(GL_TEXTURE_2D,textureId);
        
        //texture params
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR); // Mipmaps are not available for maximization
        
        //copy texture to GPU
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(),textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,textureObject->getImage());
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //save id in map
        textureIdMap[it->first] = textureId;
    }
}

void View::computeTangents(util::PolygonMesh<VertexAttrib>& tmesh)
{
    int i, j;
    vector<glm::vec4> tangents;
    vector<float> data;

    vector<VertexAttrib> vertexData = tmesh.getVertexAttributes();
    vector<unsigned int> primitives = tmesh.getPrimitives();
    int primitiveSize = tmesh.getPrimitiveSize();
    int vert1, vert2, vert3;
    if(primitiveSize == 6)
    {
        //GL_TRIANGLES_ADJACENCY
        vert1 = 0;
        vert2 = 2;
        vert3 = 4;
    }
    else
    {
        //GL_TRIANGLES
        vert1 = 0;
        vert2 = 1;
        vert3 = 2;
    }
    //initialize as 0
    for (i = 0; i < vertexData.size(); i++)
        tangents.push_back(glm::vec4(0.0f, 0.0, 0.0f, 0.0f));

    //go through all the triangles
    for (i = 0; i < primitives.size(); i += primitiveSize)
    {
        // cout<<"i: "<<i<<endl;
        int i0, i1, i2;
        i0 = primitives[i + vert1];
        i1 = primitives[i + vert2];
        i2 = primitives[i + vert3];

        //vertex positions
        data = vertexData[i0].getData("position");
        glm::vec3 v0 = glm::vec3(data[0],data[1],data[2]);

        data = vertexData[i1].getData("position");
        glm::vec3 v1 = glm::vec3(data[0],data[1],data[2]);

        data = vertexData[i2].getData("position");
        glm::vec3 v2 = glm::vec3(data[0],data[1],data[2]);

        // UV coordinates
        data = vertexData[i0].getData("texcoord");
        glm::vec2 uv0 = glm::vec2(data[0],data[1]);

        data = vertexData[i1].getData("texcoord");
        glm::vec2 uv1 = glm::vec2(data[0],data[1]);

        data = vertexData[i2].getData("texcoord");
        glm::vec2 uv2 = glm::vec2(data[0],data[1]);

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
        //orthogonalization
    for (i = 0; i < tangents.size(); i++) 
    {
        glm::vec3 t = glm::vec3(tangents[i].x,tangents[i].y,tangents[i].z);
        t = glm::normalize(t);
        data = vertexData[i].getData("normal");
        glm::vec3 n = glm::vec3(data[0],data[1],data[2]);

        glm::vec3 b = glm::cross(n,t);
        t = glm::cross(b,n);

        t = glm::normalize(t);

        tangents[i] = glm::vec4(t,0.0f);
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


void View::Resize()
{
    int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);
    projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
}

void View::updateTrackball(float deltaX, float deltaY)
{
    float sensitivity = 0.005f;
    glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0), (deltaX * sensitivity), glm::vec3(0.0f, 1.0f, 0.0f));
    if(cameraType != 2)
        rotMatrix = glm::rotate(rotMatrix, (deltaY * sensitivity), glm::vec3(1.0f, 0.0f, 0.0f));
    dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["trackball"])->premulTransformMatrix(rotMatrix);
}

void View::resetTrackball()
{
    dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["trackball"])->setTransformMatrix(glm::mat4(1.0f));
}


void View::display(sgraph::IScenegraph *scenegraph) 
{
    
    #pragma region lightSetup
    // setting up the view matrices beforehand because all render calculations are going to be on the view coordinate system.

    glm::mat4 viewMat(1.0f);
    if(cameraType == 1)
        viewMat = viewMat * glm::lookAt(glm::vec3(0.0f, 0.0f, 100.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    else if(cameraType == 2)
        viewMat = viewMat * glm::lookAt(glm::vec3(0.0f, 150.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    else if(cameraType == 3)
    {
            //Drone camera. Need to find a point that is forward(for the lookAt), find the drone co-ordinates(for the eye) and the up-direction for the up-axis
            //drone co-ordinates seem simple enough. I can just use the transform matrix with a translation of 20 in the z-axis
            //target = same as eye, the translation must be higher, so 25?
            //for the up-axis, I can convert the y axis to vec4, pre-multiply by the transformation matrix, then convert back to vec3.
            //This seems super hacky though, is there an alternate way that's easier?
            
            glm::mat4 droneTransformMatrix = dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["drone-movement"])->getTransformMatrix();
            glm::vec3 droneEye = droneTransformMatrix * glm::vec4(0.0f, 0.0f, 20.0f, 1.0f); // setting 1 as the homogenous coordinate
            // Implicit typecasts work!!!!
            glm::vec3 droneLookAt = droneTransformMatrix * glm::vec4(0.0f, 0.0f, 25.0f, 1.0f);
            glm::vec3 droneUp = droneTransformMatrix * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);//homogenous coordinate is 0.0f as the vector is an axis, not a point.
            
            viewMat = viewMat * glm::lookAt(droneEye, droneLookAt, droneUp);        
    }

    modelview.push(glm::mat4(1.0f));
    modelview.top() = modelview.top() * viewMat;
    initLights(scenegraph); // lighting scenegraph traversal happens here. I've moved this to the first because the lights need to be initialized
    // for the shadow volume pass before the rendering pass
    // initLightShaderVars(); // lighting to shader variables mapping. Saves map in LightLocation.
    modelview.pop();
    #pragma endregion
    glDepthFunc(GL_LEQUAL); // This gave me 3 hours of pain :(
    
    //rotate the propellers!
    rotatePropeller("propeller-1-rotate", glfwGetTime());
    rotatePropeller("propeller-2-rotate", glfwGetTime());
    rotatePropeller("propeller-3-rotate", glfwGetTime());
    rotatePropeller("propeller-4-rotate", glfwGetTime());

    rotate(); // drone movement rotate


    #pragma region pipeline
    // all the heavylifting happens here.
    // shadow volumes are rendered using depth fail method.
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE); // enable writing to the depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); //clear everything before starting the render loop.
    
    depthPass(scenegraph, viewMat); // set the depth buffer from the actual camera location to set up for the stencil test.
    glEnable(GL_STENCIL_TEST); // enable stencil test.
    glEnable(GL_BLEND); // for multiple lights
    glBlendFunc(GL_ONE, GL_ONE); //  Equally blend all the effects from eall the lights (is this correct?)
    for (int i = 0; i < lights.size(); i++) 
    {
        glClear(GL_STENCIL_BUFFER_BIT);
        shadowStencilPass(scenegraph, viewMat, i); // render the shadow volume into the stencil buffer.
        renderObjectPass(scenegraph, viewMat, i); // render all the objects with lighting (except ambient) into the scene. (fragments that fail the stencil test will not touch the fragment shader).
    }
    glDisable(GL_BLEND);
    glDisable(GL_STENCIL_TEST); // need to disable the stencil test for the ambient pass because all objects require ambient lighting.
    ambientPass(scenegraph, viewMat); // ambient pass for all objects.
    #pragma endregion

    //do not need this anymore.
    #pragma region silhouettePass

    #pragma endregion
    
    
    
    
    glFlush();
    glfwSwapBuffers(window);
    glfwPollEvents();
    frames++;
    double currenttime = glfwGetTime();
    if ((currenttime-time)>1.0) {
        printf("Framerate: %2.0f\r",frames/(currenttime-time));
        frames = 0;
        time = currenttime;
    }
    

}

/**
 * Render the scene into the depth buffer. Using this for depth fail and depth pass methods in stencil shadow volumes.
 */
void View::depthPass(sgraph::IScenegraph *scenegraph, glm::mat4& viewMat)
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

/**
 * Render the shadow volumes into the stencil buffer. This should run after the depth pass,
 * and the colorBuffer write must still be disabled.
 */
void View::shadowStencilPass(sgraph::IScenegraph *scenegraph, glm::mat4& viewMat, int lightIndex)
{
    glDepthMask(GL_FALSE); // do not write into the depth buffer anymore. This is so that the shadow volumes do not obstruct the actual objects.
    glEnable(GL_DEPTH_CLAMP); // Don't want to clip the back polygons.
    glDisable(GL_CULL_FACE); // Don't want the back-facing polygons to get culled. need them to increment the stencil buffer.
    glStencilFunc(GL_ALWAYS, 0, 0xff); // Always pass the stencil test, reference value 0,mask value 1(should probably use ~0)

    // For some reason, the depth-fail method breaks down for some fragments of the sphere.
    // depth fail
    // glStencilOpSeparate(GL_BACK, // for backfacing polygons
    //                     GL_KEEP, // stencil test fails - doesnt happen because stencil function is set to always pass
    //                     GL_INCR_WRAP, // stencil passes but depth fails - our required condition - increment the stencil buffer value
    //                     GL_KEEP); // both stencil and depth passes - not relevant; do nothing.
    
    // glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP); // similarly, for front facing polygons, decrement the stencil buffer
    

    // These are for depth pass. This works for now, but will fail when the camera is placed inside a shadow.
    glStencilOpSeparate(GL_FRONT,GL_KEEP, GL_KEEP,GL_INCR_WRAP);
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



void View::ambientPass(sgraph::IScenegraph *scenegraph, glm::mat4& viewMat)
{
    ambientProgram.enable();
    glEnable(GL_BLEND); // blend the ambient light.
    glBlendEquation(GL_FUNC_ADD); // blend by addition.
    glBlendFunc(GL_ONE, GL_ONE); // equal parts of existing and ambient. This is fine because the ambient shader has intensity reduced to 0.2 times.
    modelview.push(glm::mat4(1.0));
    modelview.top() = modelview.top() * viewMat;
    glUniformMatrix4fv(ambientShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // This uses only the material's ambient. Ideally it should use each light's ambient as well. Maybe later tho.
    scenegraph->getRoot()->accept(ambientRenderer);
    modelview.pop();
    ambientProgram.disable();
    glDisable(GL_BLEND);
}

void View::renderObjectPass(sgraph::IScenegraph *scenegraph, glm::mat4& viewMat, int lightIndex)
{

    renderProgram.enable();
    glDrawBuffer(GL_BACK); // Enable writing to the color buffer. This was disabled in the depth pass.
    glStencilFunc(GL_EQUAL, 0x0, 0xFF); // draw only if stencil value is 0
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP); // do not write to the stencil buffer.

    // glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT); // clearing all the buffers in the main method now.
    
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_FACE);


    modelview.push(glm::mat4(1.0));
    modelview.top() = modelview.top() * viewMat; // this should return the same behavior now.
    //lighting setup now happens at display(). This is because the lights are required for the stencil shadow pass
    // which runs before the renderObject pass.

    glUniform1i(renderShaderLocations.getLocation("numLights"), lights.size());

    // send the data for the ith light
    glm::vec4 pos = lights[lightIndex].getPosition();
    pos = lightTransformations[lightIndex] * pos;
    // position
    //adding direction for spotlight
    glm::vec4 spotDirection = lights[lightIndex].getSpotDirection();
    spotDirection = lightTransformations[lightIndex] * spotDirection;
    // Set light colors
    glUniform3fv(renderShaderLocations.getLocation("light.ambient"), 1, glm::value_ptr(lights[lightIndex].getAmbient()));
    glUniform3fv(renderShaderLocations.getLocation("light.diffuse"), 1, glm::value_ptr(lights[lightIndex].getDiffuse()));
    glUniform3fv(renderShaderLocations.getLocation("light.specular"), 1, glm::value_ptr(lights[lightIndex].getSpecular()));
    glUniform4fv(renderShaderLocations.getLocation("light.position"), 1, glm::value_ptr(pos));
    //spotlight stuff here
    glUniform1f(renderShaderLocations.getLocation("light.spotAngle"), lights[lightIndex].getSpotCutoff());
    glUniform3fv(renderShaderLocations.getLocation("light.spotDirection"), 1, glm::value_ptr(spotDirection));

    
    //send projection matrix to GPU    
    glUniformMatrix4fv(renderShaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    
    
    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);
    
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT_FACE);
    modelview.pop();
    renderProgram.disable();
}

void View::initLights(sgraph::IScenegraph *scenegraph)
{
    sgraph::LightRetriever* lightsParser = reinterpret_cast<sgraph::LightRetriever*>(lightRetriever);
    lightsParser->clearData();
    scenegraph->getRoot()->accept(lightRetriever);
    lights = lightsParser->getLights();
    lightTransformations = lightsParser->getLightTransformations();
}

void View::initLightShaderVars()
{
    lightLocations.clear();
    for (int i = 0; i < lights.size(); i++)
    {
      LightLocation ll;
      stringstream name;

      name << "light[" << i << "]";
      ll.ambient = renderShaderLocations.getLocation(name.str() + "" +".ambient");
      ll.diffuse = renderShaderLocations.getLocation(name.str() + ".diffuse");
      ll.specular = renderShaderLocations.getLocation(name.str() + ".specular");
      ll.position = renderShaderLocations.getLocation(name.str() + ".position");
      //adding spotDirection and spotAngle.
      ll.spotDirection = renderShaderLocations.getLocation(name.str() + ".spotDirection");
      ll.spotAngle = renderShaderLocations.getLocation(name.str() + ".spotAngle");
      lightLocations.push_back(ll);
    }
}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::switchShaders()
{
    // Not supporting toon shaders for shadow volumes and pbr
    // isToonShaderUsed = !isToonShaderUsed;
    // if(isToonShaderUsed)
    //     program.createProgram(string("shaders/toon.vert"),string("shaders/toon.frag"));
    // else
    //     program.createProgram(string("shaders/phong-multiple.vert"),string("shaders/phong-multiple.frag"));
    // program.enable();
    // renderShaderLocations = program.getAllShaderVariables();
    // cout<<"toon shader status: "<<isToonShaderUsed<<endl;
}



void View::closeWindow() {
    for (map<string,util::ObjectInstance *>::iterator it=objects.begin();
           it!=objects.end();
           it++) {
          it->second->cleanup();
          delete it->second;
    } 
    glfwDestroyWindow(window);

    glfwTerminate();
}

// This saves all the nodes required for dynamic transformation
void View::initScenegraphNodes(sgraph::IScenegraph *scenegraph)
{
    auto nodes = scenegraph->getNodes();
    std::vector<string> savedNodes = {"propeller-1-rotate", "propeller-2-rotate", "propeller-3-rotate", "propeller-4-rotate",
                                         "trackball", "drone-movement"};

    for(const auto& nodeName: savedNodes)
    {
        if(nodes.find(nodeName) != nodes.end())
        {
            // The node is present, save it!
            cout<<"Found : "<<nodeName<<endl; //It's finding the nodes now.
            cachedNodes[nodeName] = dynamic_cast<sgraph::TransformNode*>(nodes[nodeName]); // Can't cast to abstract class, so need to cast to pointer 
        }
    }
}


void View::rotatePropeller(string nodeName, float time)
{
    float rotationSpeed = speed * 200.0f;

    float rotationAngle = glm::radians(rotationSpeed * time);

    sgraph::RotateTransform *propellerNode = dynamic_cast<sgraph::RotateTransform*>(cachedNodes[nodeName]);
    if(propellerNode)
    {
        propellerNode->updateRotation(rotationAngle);
    }
}


void View::changePropellerSpeed(int num)
{
    //positive number = increments speed; negative number = decrements speed

    float speedSensitivity = 0.25f;
    speed += num > 0 ? speedSensitivity : -speedSensitivity;

    speed = speed <= 0 ? 0.1f : speed >= 3 ? 3.0f : speed;
}

void View::startRotation()
{

    cout<<"starting rotation!"<<endl;
    isRotating = true;
}

void View::rotate()
{
    if(!isRotating)
        return;
    
    if(rotationAngle > 360.0f)
    {
        rotationAngle = 0.0f;
        isRotating = false;
    }
    float rollSpeed = 2.0f;
    rotationAngle += rollSpeed;

    float newAngle = glm::radians(rotationAngle);
    sgraph::DynamicTransform *droneRotateNode = dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["drone-movement"]);
    droneRotateNode->postmulTransformMatrix(glm::rotate(glm::mat4(1.0), glm::radians(rollSpeed), glm::vec3(0.0f, 0.0f, 1.0f)));
    
}

/**
 * Move/Rotate the drone by passing the matrix to premultiply. This stacks on top of previous input.
 */
void View::moveDrone(int direction)
{
    glm::mat4 translateMatrix(1.0);
    float directionalSpeed = (direction > 0 ? 1.0f : -1.0f) * speed * 5.0f;
    translateMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, directionalSpeed));
    sgraph::DynamicTransform *droneTranslateNode = dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["drone-movement"]);
    droneTranslateNode->postmulTransformMatrix(translateMatrix);
}


/**
 * Set the drone's matrix to what is passed. A good idea would be to pass a rotation and a translation.
 */
void View::setDroneOrientation(glm::mat4 resetMatrix)
{
    sgraph::DynamicTransform* droneNode = dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["drone-movement"]);
    if(droneNode)
        droneNode->setTransformMatrix(resetMatrix);
}

/**
 * Pass a positive yawDir to rotate left, negative to rotate right. Similarly, positive pitchDir to rotate upwards, negative to rotate downwards
 */

void View::rotateDrone(int yawDir, int pitchDir)
{
    //Check if yaw Rotation is present
    glm::mat4 rotationMatrix(1.0f);
    if(yawDir != 0)
    {
        float yawSpeed = (yawDir > 0.0f? 1.0f : -1.0f) * 5.0f;
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(yawSpeed) , glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if(pitchDir != 0)
    {
        float pitchSpeed = (pitchDir > 0.0f? 1.0f : -1.0f) * 5.0f;
        rotationMatrix = glm::rotate(rotationMatrix, glm::radians(pitchSpeed) , glm::vec3(1.0f, 0.0f, 0.0f));
    }

    dynamic_cast<sgraph::DynamicTransform*>(cachedNodes["drone-movement"])->postmulTransformMatrix(rotationMatrix);
}

void View::changeCameraType(int cameraType)
{
    this->cameraType = cameraType;
}