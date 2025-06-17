//! [code]

#include <glad/glad.h>
#include <string>
#include "View.h"
#include "GUIView.h"
#include "Model.h"
#include "Controller.h"
#include "GUIController.h"
#include <iostream>

int main(int argc,char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    Model* model = new Model();
    GUIView* view = new GUIView();
    if(argc == 1)
    {
        GUIController controller(model,view, "");
        controller.initScenegraph();
        controller.run();
    }
    else
    {
        GUIController controller(model,view, args[1]);
        controller.initScenegraph();
        controller.run();
    }


}

//! [code]
