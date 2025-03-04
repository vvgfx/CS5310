//! [code]

#include "Box.h"
#define GLM_FORCE_SWIZZLE
#include "Stencil.h"
using namespace std;

int main(int argc,char *argv[]) {
    Stencil stencil(0,0,100,100);
    //carve out a hole from its center
    stencil.cut(20,20,60,60);
   /*

    //more complex example 
    Stencil stencil(0,0,310,310);
    
    //create a set of 10x10 windows
    for (int i=0;i<10;i++) {
        for (int j=0;j<10;j++) {
            stencil.cut(10+30*(i),10+30*(j),20,20);
        }
    }

    stencil.cut(110,80,120,55);
    */
    
}

//! [code]
