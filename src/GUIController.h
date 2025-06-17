#ifndef __GUICONTROLLER_H__
#define __GUICONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Controller.h"

class GUIController : public Controller
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
    void initScenegraph() override;

protected:
};

#endif