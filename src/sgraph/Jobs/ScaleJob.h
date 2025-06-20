#ifndef _SCALEJOB_H_
#define _SCALEJOB_H_

#include <string>
#include "../../Model.h"
#include "../Commands/ScaleCommand.h"
#include "Ijob.h"
using namespace std;

namespace job
{

    /**
     * This is an implementation of the IJob interface.
     * It uses the command design pattern to update the scale of an object.
     *
     * Note: This is a part of the controller.
     */
    class ScaleJob : public IJob
    {
    public:
        ScaleJob(string nodeName, float sx, float sy, float sz)
        {
            this->nodeName = nodeName;
            this->sx = sx;
            this->sy = sy;
            this->sz = sz;
        }

        virtual void execute(Model *m)
        {
            command::ScaleCommand* scaleCommand = new command::ScaleCommand(nodeName, sx, sy, sz);
            cout<<"Adding to command queue in job"<<endl;
            m->addToCommandQueue(scaleCommand);
        }

    private:
        float sx, sy, sz;
    };
}

#endif