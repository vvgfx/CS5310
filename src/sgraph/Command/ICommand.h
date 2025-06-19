#ifndef _ICOMMAND_H_
#define _ICOMMAND_H_

#include <string>
using namespace std;

/*
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "ScaleTransform.h"
#include "RotateTransform.h"
#include "TranslateTransform.h"
*/

#include "../SGNodeVisitor.h"

namespace command {

/**
 * This class represents the interface for a Command on scene graph nodes.
 * It extends the SGNodeVisitor, which implements the Visitor pattern.
 * 
 * 
 */
    class ICommand : public sgraph::SGNodeVisitor {
        public:
        virtual string getNodeName()=0;
        virtual void setNodeName(string name)=0;

        protected:
        string nodeName;
    };
}

#endif