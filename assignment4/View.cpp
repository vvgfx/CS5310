#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sgraph/GLScenegraphRenderer.h"
#include "VertexAttrib.h"


View::View() {

}

View::~View(){

}

void View::init(Callbacks *callbacks,map<string,util::PolygonMesh<VertexAttrib>>& meshes) 
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
    program.createProgram(string("shaders/default.vert"),
                          string("shaders/default.frag"));
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

    renderer = new sgraph::GLScenegraphRenderer(modelview,objects,shaderLocations);
    xDelta = 0.0f;
    yDelta = 0.0f;
    zDelta = 0.0f;

}

void View::Resize()
{
    int window_width,window_height;
    glfwGetFramebufferSize(window,&window_width,&window_height);
    projection = glm::perspective(glm::radians(60.0f),(float)window_width/window_height,0.1f,10000.0f);
}


void View::setLookAt(glm::mat4 lookAt){
    lookAtMatrix = lookAt;
}

void View::display(sgraph::IScenegraph *scenegraph) {
    
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
    modelview.top() = modelview.top() * lookAtMatrix;
    
    //send projection matrix to GPU    
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    

    //draw scene graph here
    scenegraph->getRoot()->accept(renderer);

    
    
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

bool View::shouldWindowClose() {
    return glfwWindowShouldClose(window);
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
    // for(auto names : nodes)
    // {
    //     cout<<"Key: "<<names.first<<endl;
    // }
    std::vector<string> savedNodes = {"propeller-1-rotate", "propeller-2-rotate", "propeller-3-rotate", "propeller-4-rotate", "drone-roll",
                                        "drone-rotate-pitch", "drone-rotate-yaw", "drone-translate"};

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
    sgraph::RotateTransform *droneRotateNode = dynamic_cast<sgraph::RotateTransform*>(cachedNodes["drone-roll"]);
    droneRotateNode->updateRotation(newAngle);
    
}

//Note to self: I have been doing this using traditional math only so far. Is there a way to do this with change in co-ordinate systems?
void View::updateRotation(float yawRot, float pitchRot)
{
    droneYaw += yawRot;
    dronePitch += pitchRot;
    // cout<<"New drone yaw: "<<droneYaw;
    //update the rotations
    sgraph::RotateTransform* yawRotationNode = dynamic_cast<sgraph::RotateTransform*>(cachedNodes["drone-rotate-yaw"]);
    yawRotationNode->updateRotation(glm::radians(droneYaw));

    sgraph::RotateTransform* pitchRotationNode = dynamic_cast<sgraph::RotateTransform*>(cachedNodes["drone-rotate-pitch"]);
    pitchRotationNode->updateRotation(glm::radians(dronePitch));

}

void View::translateDrone(int direction)
{
    //positive is forward, negative is backward


    //find the required movement
    float forwardX = sin(glm::radians(droneYaw)) * cos(glm::radians(dronePitch)) * speed * direction;
    float forwardY = sin(glm::radians(dronePitch)) * speed * direction * -1;
    float forwardZ = cos(glm::radians(droneYaw)) * cos(glm::radians(dronePitch)) * speed * direction;


    sgraph::TranslateTransform* droneTranslateNode = dynamic_cast<sgraph::TranslateTransform*>(cachedNodes["drone-translate"]);
    if(droneTranslateNode)
    {
        // cout<<"Translating!! : "<<forwardX<<" , "<<forwardY<<" , "<<forwardZ<<endl;
        droneTranslateNode->updateTransform(forwardX, forwardY, forwardZ);
    }
}