#ifndef __BOX_H__
#define __BOX_H__

#include <vector>
using namespace std;

/*
This class represents a simple 2D axis-aligned box.
*/

class Box {
    
    public:
    Box(int x,int y,int width,int height) 
    : x(x)
    , y(y)
    , width(width)
    , height(height) {
    }
    ~Box(){}
    inline int getX() {return x;}
    inline int getY() {return y;}
    inline int getWidth() {return width;}
    inline int getHeight() {return height;}

    /**
     * Find the intersection (common area) of another box with this one.
    */
    Box intersect(const Box& other) {
        int minx = this->x<other.x?other.x:this->x;
        int miny = this->y<other.y?other.y:this->y;

        int maxx = x+width>other.x+other.width?other.x+other.width:x+width;
        int maxy = y+height>other.y+other.height?other.y+other.height:y+height;
        
        return Box(minx,miny,maxx-minx,maxy-miny);
    }

/**
 * Determine if this box overlaps with another box
*/
    bool overlaps(const Box& other) const {
        if ((x>other.x+other.width) || (other.x>x+width)) {
            return false;
        }

        if ((y>other.y+other.height) || (other.y>y+height)) {
            return false;
        }

        return true;
    }

    //compute the part that is in this box but not in the other, when 
    //other is contained inside this
    vector<Box> containedDifference(const Box& other) {
        vector<Box> result;
        int x1,y1,x2,y2,w1,h1,w2,h2;
        x1 = x;
        y1 = y;
        w1 = width;
        h1 = height;

        x2 = other.x;
        y2 = other.y;
        w2 = other.width;
        h2 = other.height;


        //east
        if ((x2+w2)<(x1+w1)) {
            result.push_back(Box(x2+w2,y2,(x1+w1)-(x2+w2),(y2+h2)-y2));
        }

        //west
        if (x1<x2) {
            result.push_back(Box(x1,y2,x2-x1,(y2+h2)-y2));
        }

        //north
        if ((y2+h2)<(y1+h1)) {
            result.push_back(Box(x2,y2+h2,(x2+w2)-x2,(y1+h1)-(y2+h2)));
        }
        
        //south
        if (y1<y2) {
            result.push_back(Box({x2,y1,(x2+w2)-x2,y2-y1}));
        }
        
        //NE
        if (((x2+w2)<(x1+w1)) && ((y2+h2)<(y1+h1))) {
            result.push_back(Box(x2+w2,y2+h2,(x1+w1)-(x2+w2),(y1+h1)-(y2+h2)));
        }
        
        
        //SE
        if ((x2+w2<x1+w1) && (y1<y2)) {
            result.push_back(Box(x2+w2,y1,(x1+w1)-(x2+w2),y2-y1));
        }
        
        
        //SW
        if ((x1<x2) && (y1<y2)) {
            result.push_back(Box(x1,y1,x2-x1,y2-y1));
        }
        
        //NW
        if ((x1<x2) && ((y2+h2)<(y1+h1))) {
            result.push_back(Box(x1,y2+h2,x2-x1,(y1+h1)-(y2+h2)));
        }
        
        return result;
    }

    private:
        int x,y,width,height;

    
};

#endif