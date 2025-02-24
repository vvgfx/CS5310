//! [code]

#include "Model.h"
#include "View.h"
#include "Controller.h"

int main(int argc,char *argv[])
{
    
    Model model;
    View view;
    Controller controller(model,view);
    controller.run();


}

//! [code]
