#ifndef __STENCIL_H__
#define __STENCIL_H__

#include "Box.h"
#include <vector>
#include <iostream>
using namespace std;

/**
 * @brief This class represents a stencil constructed out of a rectangular
 * piece of paper with rectangular cutouts. All rectangles here are
 * axis-aligned.
 * 
 */
class Stencil {

    public:
        Stencil(int x,int y,int width,int height);
        
        ~Stencil();

        void cut(int x,int y,int width,int height);
        vector<Box> getRectangles();


    private:
    //Stencil(vector<Box>& boxes);
    vector<Box> boxes;

};

Stencil::Stencil(int x,int y,int width,int height) {
    this->boxes.push_back(Box(x,y,width,height));
}

/*Stencil::Stencil(vector<Box>& boxes) {
    this->boxes = boxes;
}
*/

Stencil::~Stencil() {

}

vector<Box> Stencil::getRectangles() {
    return vector<Box>(this->boxes);
}



void Stencil::cut(int x,int y,int width,int height) {
    vector<Box> result;
    Box other(x,y,width,height);

    if (this->boxes.size()==0) {
        return; //empty
    }

    // (a + b + c) \ d = (a\d) + (b\d) + (c\d) 
    // a\d = a \\ a*d  where \\ is containedDifference and * is intersect
     for (int i=0;i<boxes.size();i++) {
        if (other.overlaps(boxes[i])) {
            Box common = boxes[i].intersect(other);
            vector<Box> containedResult = boxes[i].containedDifference(common);
            result.insert(result.end(), containedResult.begin(), containedResult.end());
        }
        else {
            result.push_back(boxes[i]);
        }

    }
    boxes = result;
}


#endif