#ifndef _ICOMMAND_H_
#define _ICOMMAND_H_

#include <string>
using namespace std;

#include "../SGNodeVisitor.h"

namespace command {

/**
 * This class represents the interface for a Command on scene graph nodes.
 * It extends the SGNodeVisitor, which implements the Visitor pattern.
 * 
 * Note: This is a part of the model
 */
    class ICommand : public sgraph::SGNodeVisitor {
        public:
        /**
         * Get the unique name of the node this command is supposed to operate on.
         */
        virtual string getNodeName()=0;

        /**
         * Set the name of the node this command is supposed to operate on.
         */
        virtual void setNodeName(string name)=0;

        protected:
        string nodeName;
    };
}

#endif