#ifndef _ROTATECOMMAND_H_
#define _ROTATECOMMAND_H_

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
    class RotateCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new RotateCommand object
         *
         * @param name Name of the node this command should effect
         * @param rx rotation in x-axis
         * @param ry rotation in y-axis
         * @param rz rotation in z-axis
         */
        RotateCommand(string name, float rx, float ry, float rz, float angleInRadians)
        {
            this->nodeName = name;
            this->rx = rx;
            this->ry = ry;
            this->rz = rz;
            this->angleInRadians = angleInRadians;
        }

        /**
         * @brief Update Rotation on rotateNode
         *
         * @param rotateNode
         */
        void visitRotateTransform(sgraph::RotateTransform *rotateNode)
        {
            rotateNode->updateRotation(angleInRadians);
            rotateNode->updateRotationAxis(rx, ry, rz);
        }

    private:
        float rx, ry, rz, angleInRadians;
    };
}

#endif