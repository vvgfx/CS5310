#ifndef _INSERTSCALEJOB_H_
#define _INSERTSCALEJOB_H_

#include <string>
#include "../../Model.h"
#include "../Commands/InsertScaleCommand.h"
#include "Ijob.h"
using namespace std;

namespace job
{

    /**
     * This is an implementation of the IJob interface.
     * It uses the command design pattern to insert a scale node under the given node.
     *
     * Note: This is a part of the controller.
     */
    class InsertScaleJob : public IJob
    {
    public:
        InsertScaleJob(string nodeName, float sx, float sy, float sz)
        {
            this->nodeName = nodeName;
            this->sx = sx;
            this->sy = sy;
            this->sz = sz;
        }

        virtual void execute(Model *m)
        {
            command::InsertScaleCommand* scaleCommand = new command::InsertScaleCommand(nodeName, sx, sy, sz, m->getScenegraph());
            cout<<"Adding to command queue in job"<<endl;
            m->addToCommandQueue(scaleCommand);
        }

    private:
        float sx, sy, sz;
    };
}

#endif