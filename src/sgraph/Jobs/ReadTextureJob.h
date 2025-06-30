#ifndef _READTEXTUREJOB_H_
#define _READTEXTUREJOB_H_

#include <string>
#include "../../Model.h"
// #include "../Commands/InsertGroupCommand.h"
#include "../PPMImageLoader.h"
#include "Ijob.h"
using namespace std;

namespace job
{

    /**
     * This is an implementation of the IJob interface.
     * It uses the command design pattern to read a new texture from disk.
     *
     * Note: This is a part of the controller.
     */
    class ReadTextureJob : public IJob
    {
    public:
        ReadTextureJob(string texName, string texPath)
        {
            this->textureName = texName;
            this->texturePath = texPath;
        }

        virtual void execute(Model *m)
        {
            // command::InsertGroupCommand* groupCommand = new command::InsertGroupCommand(nodeName, newNodeName, m->getScenegraph());
            // cout<<"Adding to command queue in job"<<endl;
            // m->addToCommandQueue(groupCommand);
            cout<<"About to load textures!"<<endl;
            sgraph::PPMImageLoader textureLoader;
            textureLoader.load(texturePath);
            util::TextureImage* texImage = new util::TextureImage(textureLoader.getPixels(), textureLoader.getWidth(), textureLoader.getHeight(), textureName);
            cout<<"Loaded textures!"<<endl;
            
        }

    private:
        string textureName, texturePath;
    };
}

#endif