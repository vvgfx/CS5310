#ifndef __GUICALLBACKS_H__
#define __GUICALLBACKS_H__

#include "Callbacks.h"
#include "sgraph/Jobs/IJob.h"

/**
 * This class sets GUI callbacks for the GUIController and GUIView.
 */
class GUICallbacks {
    public:

    virtual void receiveJob(job::IJob* job)=0;
    virtual void loadScene(string sceneName)=0;
    virtual void saveScene(string scenePath)=0;
    virtual void loadMesh(string meshName, util::PolygonMesh<VertexAttrib>& polymesh)=0;
};

#endif