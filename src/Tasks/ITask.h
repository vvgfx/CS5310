#ifndef _ITASK_H_
#define _ITASK_H_

#include <string>
// #include "../Model.h"
using namespace std;

class Model;
namespace task {

/**
 * This is an interface for a task on the main thread.
 * Sometimes you need something to be done on the main thread, like memory transfer
 * from CPU to GPU. These tasks that cannot be run on a parallel thread, so add them here.
 * They will be executed before the start of the next frame
 * 
 * Add this to the taskqueue by calling model->addtoTaskQueue()
 * 
 * Note: This is a part of the model.
 */
    class ITask {
        public:

        /**
         * This execute method is run on the main thread before the start of next frame.
         */
        virtual void execute(Model* m)=0;

        protected:
        string nodeName;
    };
}

#endif