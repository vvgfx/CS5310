#ifndef _SCALECOMMAND_H_
#define _SCALECOMMAND_H_

#include "ICommand.h"
#include "AbstractCommand.h"
#include "../GroupNode.h"
#include "../LeafNode.h"
#include "../TransformNode.h"
#include "../RotateTransform.h"
#include "../ScaleTransform.h"
#include "../TranslateTransform.h"
#include "../DynamicTransform.h"
#include <stack>
#include <iostream>
using namespace std;

namespace command
{
    /**
     * This command uses the command design pattern to make updates to a scenegraph's nodes
     *
     */
    class ScaleCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new ScaleCommand object
         *
         * @param name Name of the node this command should effect
         * @param sx scale in x-axis
         * @param sy scale in y-axis
         * @param sz scale in z-axis
         */
        ScaleCommand(string name, float sx, float sy, float sz)
        {
            this->nodeName = name;
            this->sx = sx;
            this->sy = sy;
            this->sz = sz;
        }

        /**
         * @brief Visit Scale Node on AbstractCommand
         *
         * @param scaleNode
         */
        void visitScaleTransform(sgraph::ScaleTransform *scaleNode)
        {
            scaleNode->updateScale(sx, sy, sz);
        }

    private:
        float sx, sy, sz;
    };
}

#endif