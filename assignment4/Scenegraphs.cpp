//! [code]

#include <glad/glad.h>
#include <string>
#include "View.h"
#include "Model.h"
#include "Controller.h"
#include <iostream>

int main(int argc,char *argv[]) {
    std::vector<std::string> args(argv, argv + argc);
    cout<<"reached here: "<<argc<<endl;
    Model model;
    View view;
    if(argc == 1)
    {
        Controller controller(model,view, "");
        controller.run();
    }
    else
    {
        Controller controller(model,view, args[1]);
        controller.run();
    }


}

//! [code]
