#ifndef __GUICONTROLLER_H__
#define __GUICONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Controller.h"
#include "GUICallbacks.h"
#include "sgraph/Jobs/IJob.h"

class GUIController : public Controller, public GUICallbacks
{
public:
    GUIController(Model* m, View* v, string textFile);
    ~GUIController();
    void run();

    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void error_callback(int error, const char *description);
    virtual void onMouseInput(int button, int action, int mods);
    virtual void onCursorMove(double newXPos, double newYPos);
    virtual void saveScene(string fileName);

    virtual void processInputs();
    void initScenegraph() override;

    virtual void receiveJob(job::IJob* job);

    // for restart
    virtual void loadScene(string newScenegraphName);

    // for runtime model
    virtual void loadMesh(string meshName, util::PolygonMesh<VertexAttrib>& polymesh);

protected:

    bool wFlag, sFlag, aFlag, dFlag;
};

#endif