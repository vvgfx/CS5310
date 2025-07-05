#ifndef _ADD_MESH_TASK_H_
#define _ADD_MESH_TASK_H_

#include <string>
#include "../Model.h"
#include "../sgraph/PPMImageLoader.h"
#include "ITask.h"
#include "../GUICallbacks.h"
using namespace std;

namespace task
{

    /**
     * This is an implementation of the ITask interface.
     * It uses the task queue in the model to add models to the pipeline.
     *
     */
    class AddMeshTask : public ITask
    {
    public:
        AddMeshTask(Model* m, string meshName, string meshPath, GUICallbacks* callbacks, util::PolygonMesh<VertexAttrib>& polymesh)
        {
            this->mesh = polymesh;
            this->meshName = meshName;
            this->meshPath = meshPath;
            this->m = m;
            this->callbacks = callbacks;
        }

        virtual void execute()
        {
            //first add to model
            m->addNewMesh(meshName, meshPath, mesh);
            // now add to the pipeline via the callbacks interface.
            callbacks->loadMesh(meshName, mesh);
        }

    private:
    util::PolygonMesh<VertexAttrib> mesh;
    Model* m;
    string meshName, meshPath;
    GUICallbacks* callbacks;
    };
}

#endif