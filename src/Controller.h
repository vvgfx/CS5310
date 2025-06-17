#ifndef __CONTROLLER_H__
#define __CONTROLLER_H__

#include "View.h"
#include "Model.h"
#include "Callbacks.h"

class Controller: public Callbacks
{
public:
    Controller(Model* m,View* v, string textFile);
    ~Controller();
    void run();

    virtual void reshape(int width, int height);
    virtual void dispose();
    virtual void onkey(int key, int scancode, int action, int mods);
    virtual void error_callback(int error, const char* description);
    virtual void onMouseInput(int button, int action, int mods);
    virtual void onCursorMove(double newXPos, double newYPos);
    virtual void initScenegraph();

protected:
    View* view;
    Model* model;
    bool mousePressed = false;
    double oldXPos, oldYPos, newXPos, newYPos;
    string textfile;
};

#endif