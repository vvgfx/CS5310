#ifndef _TRANSFER_TEXTURE_TASK_H_
#define _TRANSFER_TEXTURE_TASK_H_

#include <string>
#include "../Model.h"
#include "../sgraph/PPMImageLoader.h"
#include "ITask.h"
using namespace std;

namespace task
{

    /**
     * This is an implementation of the ITask interface.
     * It uses the task queue in the model to transfer textures from CPU to GPU memory.
     *
     */
    class TransferTextureTask : public ITask
    {
    public:
        TransferTextureTask(Model* m, string textureName, util::TextureImage* texImage)
        {
            this->textureName = textureName;
            this->texImage = texImage;
            this->m = m;
        }

        virtual void execute()
        {
            m->addTexture(textureName, texImage);
        }

    private:
    Model* m;
    string textureName;
    util::TextureImage* texImage;
    };
}

#endif