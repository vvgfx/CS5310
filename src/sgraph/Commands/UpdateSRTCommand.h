#ifndef _UPDATESRTCOMMAND_H_
#define _UPDATESRTCOMMAND_H_

#include "ICommand.h"
#include "AbstractCommand.h"
#include "../GroupNode.h"
#include "../LeafNode.h"
#include "../TransformNode.h"
#include "../RotateTransform.h"
#include "../ScaleTransform.h"
#include "../TranslateTransform.h"
#include "../DynamicTransform.h"
#include "../SRTNode.h"
#include <stack>
#include <iostream>
using namespace std;

namespace command
{
    /**
     * This command uses the command design pattern to make updates to a scenegraph's nodes
     *
     */
    class UpdateSRTCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new UpdateSRTCommand object
         *
         * @param name Name of the node this command should effect
         * @param sx scale in x-axis
         * @param sy scale in y-axis
         * @param sz scale in z-axis
         * @param rx rotation in x-axis (radians)
         * @param ry rotation in y-axis (radians)
         * @param rz rotation in z-axis (radians)
         * @param tx translation in x-axis
         * @param ty translation in y-axis
         * @param tz translation in z-axis
         */
        UpdateSRTCommand(string name, float sx, float sy, float sz, float rx, float ry, float rz, float tx, float ty, float tz)
        {
            this->nodeName = name;
            // scale
            this->sx = sx;
            this->sy = sy;
            this->sz = sz;

            // rotate
            this->rx = rx;
            this->ry = ry;
            this->rz = rz;

            // translate
            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
        }

        /**
         * @brief Update Rotation on rotateNode
         *
         * @param rotateNode
         */
        void visitSRTNode(sgraph::SRTNode *srtNode)
        {
            srtNode->setAbsoluteTransform(tx, ty, tz, rx, ry, rz, sx, sy, sz);
        }

    private:
        float rx, ry, rz;
        float sx, sy, sz;
        float tx, ty, tz;
    };
}

#endif