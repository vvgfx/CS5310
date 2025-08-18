#ifndef _COMPUTEPROGRAM_H_
#define _COMPUTEPROGRAM_H_

#include <glad/glad.h>
#include "ShaderLocationsVault.h"
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

namespace util
{	

   /*
    * This class encapsulates in a very basic way, a GLSL shader compute program.
    */

class ComputeProgram
{
    class ShaderInfo
    {
        friend class ComputeProgram;
    private:
       // is it a vertex shader, a fragment shader, a geometry shader,
       // a tesselation shader or none of the above?
        int type;
        string  filename; //the file that stores this shader
        int shader; //the ID for this shader after it has been compiled


    public:
        ShaderInfo(int type,string filename,int shader)
        {
            this->type = type;
            this->filename = filename;
            this->shader = shader;
        }

        ShaderInfo()
        {
            this->type = 0;
            this->filename = "";
            this->shader = 0;
        }

        ~ShaderInfo(){}
    };

public:
    ComputeProgram()
    {
        program = -1;
        enabled = false;
    }

    ~ComputeProgram()
    {
    }

    /*
     * Return the program ID of this program
     * \return the program ID
     */
    int getProgram()
    {
        return program;
    }

    /*
     * Create a new shader program with the given
     * source code for a vertex shader and fragment
     * shader
     * \param to provide access to opengl functions
     * \param computeShaderFile the file for the source code for the compute shader.
     *        This file must be placed within the resources of this project
     *        for portability purposes.
     * \throws runtime_error if any error is encountered
     */
    void createProgram(string computeShaderFile)
    {

        releaseShaders();

        shader = ShaderInfo(GL_COMPUTE_SHADER,computeShaderFile,-1);

        program = createShaders();
    }

    /*
     * Releases the resources (i.e. memory locations) for this shader. This must be called
     * only after the shader program is used, and will not be used again.
     */
    void releaseShaders()
    {
        if (shader.shader!=0)
        {
            glDeleteShader(shader.shader);
            shader.shader = 0;
        }
        if (program>=0)
            glDeleteProgram(program);
        program = 0;
    }

    /*
     * Make this program "current". All data henceforth will be sent to this program.
     */
    void enable()
    {
        glUseProgram(program);
        enabled = true;
    }

    /*
     * Disable this program.
     */
    void disable()
    {
        glUseProgram(0);
        enabled = false;
    }

    /*
     * Returns the table of all shader variables for this shader.
     *
     *
     * \return a ShaderLocationsVault object that represents the table of all shader variables
     */

    ShaderLocationsVault getAllShaderVariables()
    {
        ShaderLocationsVault vault;
        int numVars;

        glGetProgramiv(program, GL_ACTIVE_UNIFORMS,&numVars);
        for (int i=0;i<numVars;i++)
        {
            GLint size;
            GLsizei length;
            GLenum type;
            char nameVar[80];
            glGetActiveUniform(program,i,80,&length,&size,&type,nameVar);
            string name = nameVar;


            vault.add(name,getUniformLocation(nameVar));
        }

        glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES,&numVars);
        for (int i=0;i<numVars;i++)
        {
            GLint size;
            GLsizei length;
            GLenum type;
            char nameVar[80];
            glGetActiveAttrib(program,i,80,&length,&size,&type,nameVar);
            string name = nameVar;

            vault.add(name,getAttributeLocation(name));
        }
        return vault;

    }

private:
    int getUniformLocation(const string& name)
    {
        bool enabledStatus = enabled;

        if (!enabledStatus) //if not enabled, enable it
            enable();

        int id = glGetUniformLocation(program,name.c_str());

        if (!enabledStatus) //if it was not enabled, disable it again
            disable();

        return id;
    }

    int getAttributeLocation(const string& name)
    {
        bool enabledStatus = enabled;

        if (!enabledStatus) //if not enabled, enable it
            enable();

        int id = glGetAttribLocation(program,name.c_str());

        if (!enabledStatus) //if it was not enabled, disable it again
            disable();

        return id;
    }


    int createShaders()
    {
        ifstream file;
        GLint linked,compiled;

        program = glCreateProgram();

        file.open(shader.filename.c_str());


        if (!file.is_open())
        {
            stringstream str;
            str << "Shader " << shader.filename << "not found or could not be opened!" << endl;
            throw runtime_error(str.str());
        }

        string source,line;


        getline(file,line);
        while (!file.eof())
        {
            source = source + "\n" + line;
            getline(file,line);
        }
        file.close();


        const char *codev = source.c_str();


        shader.shader = glCreateShader(shader.type);
        glShaderSource(shader.shader,1,&codev,NULL);
        glCompileShader(shader.shader);
        glGetShaderiv(shader.shader,GL_COMPILE_STATUS,&compiled);

        if (!compiled)
        {
            string error_message = printShaderInfoLog(shader.shader);
            releaseShaders();
            throw runtime_error(error_message);
        }
        glAttachShader(program, shader.shader);

        glLinkProgram(program);
        glGetProgramiv(program,GL_LINK_STATUS,&linked);

        if (!linked)
        {
            string error_message = printShaderInfoLog(program);
            releaseShaders();
            throw runtime_error(error_message);
        }

        return program;
    }

    string printShaderInfoLog(GLuint shader)
    {
        int infologLen = 0;
        int charsWritten = 0;
        GLubyte *infoLog;
        stringstream str;

        glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infologLen);
        if (infologLen>0)
        {
            infoLog = new GLubyte[infologLen];
            if (infoLog != NULL)
            {
                glGetShaderInfoLog(shader,infologLen,&charsWritten,(char *)infoLog);
                for (int i=0;i<charsWritten;i++)
                    str << infoLog[i];
                str << endl;
                delete []infoLog;
            }

        }
        return str.str();
    }

private:
    int program;
    ShaderInfo shader;
    bool enabled;
    GLenum a;
};
}
#endif

