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

    window = glfwCreateWindow(800, 800, "Hello GLFW: Per-vertex coloring", NULL, NULL);
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

    // create the shader program
    program.createProgram(string("shaders/phong-multiple.vert"),
                          string("shaders/phong-multiple.frag"));
    // assuming it got created, get all the shader variables that it uses
    // so we can initialize them at some point
    // enable the shader program
    program.enable();
    shaderLocations = program.getAllShaderVariables();

    
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

    shaderVarsToVertexAttribs["vPosition"] = "position";
    shaderVarsToVertexAttribs["vNormal"] = "normal";
    shaderVarsToVertexAttribs["vTexCoord"] = "texcoord";
    
    
    for (typename map<string,util::PolygonMesh<VertexAttrib> >::iterator it=meshes.begin();
           it!=meshes.end();
           it++) {
        util::ObjectInstance * obj = new util::ObjectInstance(it->first);
        obj->initPolygonMesh(shaderLocations,shaderVarsToVertexAttribs,it->second);
        objects[it->first] = obj;
    }
    
	int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);

    //prepare the projection matrix for perspective projection
	projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
    glViewport(0, 0, window_width,window_height);

    frames = 0;
    time = glfwGetTime();

    initTextures(texMap);
    renderer = new sgraph::GLScenegraphRenderer(modelview, objects, shaderLocations, textureIdMap);
    lightRetriever = new sgraph::LightRetriever(modelview);
}

void View::initTextures(map<string, util::TextureImage*> textureMap)
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
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR_MIPMAP_LINEAR);

        //copy texture to GPU
	    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureObject->getWidth(),textureObject->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE,textureObject->getImage());
        glGenerateMipmap(GL_TEXTURE_2D);

        //save id in map
        textureIdMap[it->first] = textureId;
    }
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


void View::display(sgraph::IScenegraph *scenegraph) {
    // cout<<"Entered here!"<<endl;
    program.enable();
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT_FACE);

    
    //rotate the propellers!
    rotatePropeller("propeller-1-rotate", glfwGetTime());
    rotatePropeller("propeller-2-rotate", glfwGetTime());
    rotatePropeller("propeller-3-rotate", glfwGetTime());
    rotatePropeller("propeller-4-rotate", glfwGetTime());

    rotate();
    
    modelview.push(glm::mat4(1.0));
    if(cameraType == 1)
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0.0f, 300.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    else if(cameraType == 2)
        modelview.top() = modelview.top() * glm::lookAt(glm::vec3(0.0f, 150.0f, 300.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
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
            
            modelview.top() = modelview.top() * glm::lookAt(droneEye, droneLookAt, droneUp);        
    }

    initLights(scenegraph);
    initShaderVars();
    glUniform1i(shaderLocations.getLocation("numLights"), lights.size());

    for (int i = 0; i < lights.size(); i++) {
    
        glm::vec4 pos = lights[i].getPosition();
        // cout<<"Light position : "<<i<<pos.x<<" , "<<pos.y<<" , "<<pos.z<<endl;
        pos = lightTransformations[i] * pos;
        // position
        
        //adding direction for spotlight
        glm::vec4 spotDirection = lights[i].getSpotDirection();
        spotDirection = lightTransformations[i] * spotDirection;
        // Set light colors
        glUniform3fv(lightLocations[i].ambient, 1, glm::value_ptr(lights[i].getAmbient()));
        glUniform3fv(lightLocations[i].diffuse, 1, glm::value_ptr(lights[i].getDiffuse()));
        glUniform3fv(lightLocations[i].specular, 1, glm::value_ptr(lights[i].getSpecular()));
        glUniform4fv(lightLocations[i].position, 1, glm::value_ptr(pos));
        //spotlight stuff here
        glUniform1f(lightLocations[i].spotAngle, lights[i].getSpotCutoff());
        glUniform3fv(lightLocations[i].spotDirection, 1, glm::value_ptr(spotDirection));
    }
        
    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));

    //enable textures before starting the renderer.
    glEnable(GL_TEXTURE_2D);

    // Update here for any active textures and their mappings
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(shaderLocations.getLocation("image"), 0);

    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT_FACE);

    
    
    modelview.pop();
    glFlush();
    program.disable();
    
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

void View::initLights(sgraph::IScenegraph *scenegraph)
{
    sgraph::LightRetriever* lightsParser = reinterpret_cast<sgraph::LightRetriever*>(lightRetriever);
    lightsParser->clearData();
    scenegraph->getRoot()->accept(lightRetriever);
    lights = lightsParser->getLights();
    lightTransformations = lightsParser->getLightTransformations();

    // cout<<"Light count: "<<lights.size()<<endl;
}

void View::initShaderVars()
{
    lightLocations.clear();
    for (int i = 0; i < lights.size(); i++)
    {
      LightLocation ll;
      stringstream name;

      name << "light[" << i << "]";
      ll.ambient = shaderLocations.getLocation(name.str() + "" +".ambient");
      ll.diffuse = shaderLocations.getLocation(name.str() + ".diffuse");
      ll.specular = shaderLocations.getLocation(name.str() + ".specular");
      ll.position = shaderLocations.getLocation(name.str() + ".position");
      //adding spotDirection and spotAngle.
      ll.spotDirection = shaderLocations.getLocation(name.str() + ".spotDirection");
      ll.spotAngle = shaderLocations.getLocation(name.str() + ".spotAngle");
      lightLocations.push_back(ll);
    }
}

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
}

void View::switchShaders()
{
    isToonShaderUsed = !isToonShaderUsed;
    if(isToonShaderUsed)
        program.createProgram(string("shaders/toon.vert"),string("shaders/toon.frag"));
    else
        program.createProgram(string("shaders/phong-multiple.vert"),string("shaders/phong-multiple.frag"));
    program.enable();
    shaderLocations = program.getAllShaderVariables();
    cout<<"toon shader status: "<<isToonShaderUsed<<endl;
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