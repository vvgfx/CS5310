#ifndef _INSERTGROUPJOB_H_
#define _INSERTGROUPJOB_H_

#include <string>
#include "../../Model.h"
#include "../Commands/InsertGroupCommand.h"
#include "Ijob.h"
using namespace std;

namespace job
{

    /**
     * This is an implementation of the IJob interface.
     * It uses the command design pattern to insert a translate node under the given node.
     *
     * Note: This is a part of the controller.
     */
    class InsertGroupJob : public IJob
    {
    public:
        InsertGroupJob(string nodeName)
        {
            this->nodeName = nodeName;
            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
        }

        virtual void execute(Model *m)
        {
            command::InsertGroupCommand* groupCommand = new command::InsertGroupCommand(nodeName, m->getScenegraph());
            cout<<"Adding to command queue in job"<<endl;
            m->addToCommandQueue(groupCommand);
        }

    private:
        float tx, ty, tz;
    };
}

#endif