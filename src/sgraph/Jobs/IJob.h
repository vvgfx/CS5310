#ifndef _IJOB_H_
#define _IJOB_H_

#include <string>
#include "../../Model.h"
using namespace std;

namespace job {

/**
 * This is an interface for a Job on scene graph nodes.
 * It uses the command design pattern to execute heavy workloads on parallel-threads
 * 
 * Note: This is a part of the controller.
 */
    class IJob {
        public:

        /**
         * This should do any sort of processing required in a parallel thread, and once done,
         * create a ICommand node and add it to the command-queue using m->addToCommandQueue()
         */
        virtual void execute(Model* m)=0;

        protected:
        string nodeName;
    };
}

#endif