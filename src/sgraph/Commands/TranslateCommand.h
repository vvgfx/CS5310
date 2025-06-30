#ifndef _TRANSLATECOMMAND_H_
#define _TRANSLATECOMMAND_H_

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
    class TranslateCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new TranslateCommand object
         *
         * @param name Name of the node this command should effect
         * @param tx translation in x-axis
         * @param ty translation in y-axis
         * @param tz translation in z-axis
         */
        TranslateCommand(string name, float tx, float ty, float tz)
        {
            this->nodeName = name;
            this->tx = tx;
            this->ty = ty;
            this->tz = tz;
        }

        /**
         * @brief Update Translation on translateNode
         *
         * @param translateNode
         */
        void visitTranslateTransform(sgraph::TranslateTransform *translateNode)
        {
            translateNode->setAbsoluteTransform(tx, ty, tz);
        }

    private:
        float tx, ty, tz;
    };
}

#endif