//first example created by Amit Shesh, starting from the first example in glfw. Citation below for original example.

//========================================================================
// Simple GLFW example
// Copyright (c) Camilla Löwy <elmindreda@glfw.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================
//! [code]

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdlib.h>
#include <stdio.h>

float vertexDataAsFloats[] = {-100,-100,100,-100,100,100,-100,-100,100,
                100,-100,100};
static const char* vertex_shader_text =
"#version 330\n"
"in vec2 vPosition;\n"
"uniform vec4 vColor;\n"
"uniform mat4 projection;\n"
"out vec4 outColor;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * vec4(vPosition,0.0,1.0);\n"
"    outColor = vColor;\n"
"}\n";

static const char* fragment_shader_text =
"#version 140\n"
"in vec4 outColor;\n"
"out vec4 fColor;\n"
"void main()\n"
"{\n"
"    fColor = outColor;\n"
"}\n";

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void)
{
    GLFWwindow* window;
    GLuint vertex_buffer, vertex_array,vertex_shader, fragment_shader, program;
    GLint projection_location, vpos_location,color_location;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(400, 400, "Hello GLFW: Simple", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    // NOTE: OpenGL error checks have been omitted for brevity
	
	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
	
	glUseProgram(program);
    glGenVertexArrays(1,&vertex_array);
    glBindVertexArray(vertex_array);
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), vertexDataAsFloats, GL_STATIC_DRAW);

    

    projection_location = glGetUniformLocation(program, "projection");
	printf("projection: %d\n",projection_location);
    vpos_location = glGetAttribLocation(program, "vPosition");
	printf("vPosition: %d\n",vpos_location);
	color_location = glGetUniformLocation(program,"vColor");
	printf("vColor: %d\n",color_location);
    
	float ratio;
	int width, height;
	glm::mat4 p;

	glfwGetFramebufferSize(window, &width, &height);

	glViewport(0, 0, width, height);
	

	p = glm::ortho(-150.0f, 150.0f, -150.0f, 150.0f);
        
		


        
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(p));

    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          0, (void*) 0);
    
	glm::vec4 color = glm::vec4(1.0f,0.0f,0.0f,1.0f);
	glUniform4fv( //the color is a uniform variable in the shader
				 //4f indicates this will be specified as 3 float values
				 //v means the three values will be specified in an array
			color_location, //location in shader
			1, //only one value should be read from the array below
			glm::value_ptr(color)); //convenience function to convert
									//glm::vec4 to float array
	
	



    while (!glfwWindowShouldClose(window))
    {
		
        
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}

//! [code]
