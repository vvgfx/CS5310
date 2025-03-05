#include "View.h"
#include <cstdio>
#include <cstdlib>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "VertexAttribWithColor.h"
#include <stack>

View::View() {

}

View::~View(){

}

void View::init(Callbacks *callbacks,vector<util::PolygonMesh<VertexAttrib> >& meshes)
{
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(1000 , 1000, "Spirograph!", NULL, NULL);
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


    //now we create an object that will be used to render this mesh in opengl
    /*
     * now we create an ObjectInstance for it.
     * The ObjectInstance encapsulates a lot of the OpenGL-specific code
     * to draw this object
     */

    /* so in the mesh, we have some attributes for each vertex. In the shader
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

    // currently there are only two per-vertex attribute: position and color
    shaderVarsToVertexAttribs["vPosition"] = "position";

    for (int i=0;i<meshes.size();i++) {
        util::ObjectInstance *obj = new util::ObjectInstance("triangles");
        obj->initPolygonMesh<VertexAttrib>(
            program,                    // the shader program
            shaderLocations,            // the shader locations
            shaderVarsToVertexAttribs,  // the shader variable -> attrib map
            meshes[i]);                      // the actual mesh object

        objects.push_back(obj);

    }

    // Adding path stuff
    // Create VAO and VBO for the path
    glGenVertexArrays(1, &pathVAO);
    glGenBuffers(1, &pathVBO);

    glBindVertexArray(pathVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    // Somehow, the position location seems to be 0 (looking at the shader program)
    glEnableVertexAttribArray(shaderLocations.getLocation("vPosition"));
    glVertexAttribPointer(shaderLocations.getLocation("vPosition"), 2, GL_FLOAT, GL_FALSE,
        sizeof(glm::vec2), (void*) 0);

    glBindVertexArray(0); // Seems to be good OpenGL practice?

	int width,height;
    glfwGetFramebufferSize(window,&width,&height);

    //prepare the projection matrix for orthographic projection
	projection = glm::ortho(-width/2.0f,width/2.0f,-height/2.0f,height/2.0f);
    glViewport(0, 0, width,height);

    frames = 0;
    time = glfwGetTime();

}



void View::display()
{
    glm::vec4 outerColor = glm::vec4(1,0,0,1);
    glm::vec4 innerColor = glm::vec4(0,0,1,1);
    glm::vec4 seedColor = glm::vec4(0,1,0,1);
    glm::vec4 pathColor = glm::vec4(0,1,0,1);
    float ROTATION_SPEED = 400.0f;

    float revolution = glfwGetTime() * ROTATION_SPEED;
    float rotation  = glm::radians(revolution) * OUTER_RADIUS / INNER_RADIUS;
    float radRotation = glm::radians(revolution);
    program.enable();
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    //send projection matrix to GPU
    glUniformMatrix4fv(shaderLocations.getLocation("projection"), 1, GL_FALSE, glm::value_ptr(projection));
    modelview = glm::mat4(1.0);
    //send modelview matrix to GPU

    // draw outer circle
    glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
    glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(outerColor));
    for (int i=0;i<objects.size();i++) {
        objects[i]->draw();
    }

    // draw inner circle
    modelview = glm::translate(modelview, glm::vec3((OUTER_RADIUS - INNER_RADIUS) * cos(radRotation), (OUTER_RADIUS - INNER_RADIUS) * sin(radRotation), 0));
    modelview = glm::rotate(modelview, rotation, glm::vec3(0, 0, 1));
    modelview = glm::scale(modelview, glm::vec3(INNER_RADIUS / OUTER_RADIUS, INNER_RADIUS / OUTER_RADIUS, 1.0));
    glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
    glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(innerColor));
    for (int i=0;i<objects.size();i++) {
        objects[i]->draw();
    }

    // draw seed
    modelview = glm::translate(modelview, glm::vec3(200.0f, 0 , 0));
    modelview = glm::scale(modelview, glm::vec3(0.01, 0.01, 1.0));
    glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
    glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(seedColor));
    for (int i=0;i<objects.size();i++) {
        objects[i]->draw();
    }

    if(seedPath.empty() || glm::distance(seedPath.back() , glm::vec2(modelview[3][0], modelview[3][1])) > 2.0f)
        seedPath.push_back(glm::vec2(modelview[3][0], modelview[3][1]));

    //now draw the seedPath
    if(seedPath.size() > 1)
    {
        // need to reset modelview as I already have all the plot positions directly
        modelview = glm::mat4(1.0);

        glBindVertexArray(pathVAO);
        glBindBuffer(GL_ARRAY_BUFFER, pathVBO);
        glBufferData(GL_ARRAY_BUFFER, seedPath.size() * sizeof(glm::vec2), seedPath.data(), GL_STATIC_DRAW);

        glUniformMatrix4fv(shaderLocations.getLocation("modelview"), 1, GL_FALSE, glm::value_ptr(modelview));
        glUniform4fv(shaderLocations.getLocation("vColor"),1,glm::value_ptr(seedColor));
        glDrawArrays(GL_LINE_STRIP, 0, seedPath.size());
        // glDrawElements(GL_LINE_STRIP, seedPath.size(), GL_UNSIGNED_INT, (GLvoid *)0);
        glBindVertexArray(0);
    }

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
    for (int i=0;i<objects.size();i++) {
        objects[i]->cleanup();
        delete objects[i];
    }
    objects.clear();
    glfwDestroyWindow(window);

    glfwTerminate();
}
