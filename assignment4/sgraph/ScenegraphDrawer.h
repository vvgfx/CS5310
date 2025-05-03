#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include <ShaderProgram.h>
#include <ShaderLocationsVault.h>
#include "ObjectInstance.h"
#include <stack>
#include <iostream>

namespace sgraph {
    /**
     * This visitor implements drawing the scene graph using OpenGL
     *
     */
    class ScenegraphDrawer : public SGNodeVisitor
    {
        public:
        ScenegraphDrawer()
        {
            numSpaces = 0;
        }

        void visitGroupNode(GroupNode *node)
        {
            cout<<string(numSpaces, ' ')<<"- "<<node->getName()<<" (Group)"<<endl;
            auto children = node->getChildren();
            numSpaces +=4;
            for(int i = 0; i < children.size(); i++)
            {
                children[i]->accept(this);
            }
            numSpaces -=4;
        }

        void visitLeafNode(LeafNode *node)
        {
            cout<<string(numSpaces, ' ')<<"- "<<node->getName()<<" (Leaf)"<<endl;

        }

        void transformNodeHelper(TransformNode *node, string name)
        {
            cout<<string(numSpaces, ' ')<<"- "<<node->getName()<<" ("<<name<<")"<<endl;
            auto children = node->getChildren();
            numSpaces +=4;
            for(int i = 0; i < children.size(); i++)
            {
                children[i]->accept(this);
            }
            numSpaces -=4;
        }

        void visitTransformNode(TransformNode *node)
        {
            transformNodeHelper(node, "Transform");
        }

        void visitScaleTransform(ScaleTransform *node)
        {
            glm::vec3 scale = node->getScale();
            string s = "Scale: " + to_string(scale.x) + " , " + to_string(scale.y) + " , " + to_string(scale.z) + " ";
            transformNodeHelper(node, s);
        }
        
        void visitTranslateTransform(TranslateTransform *node)
        {
            glm::vec3 translate = node->getTranslate();
            string s = "Translate: " + to_string(translate.x) + " , " + to_string(translate.y) + " , " + to_string(translate.z) + " ";
            transformNodeHelper(node, s);
        }

        void visitRotateTransform(RotateTransform *node)
        {
            glm::vec3 rotate = node->getRotationAxis();
            string s = "Rotate: " + to_string(rotate.x) + " , " + to_string(rotate.y) + " , " + to_string(rotate.z) + " ";
            s += to_string(node->getAngleInRadians());
            transformNodeHelper(node, s);
        }

        private:
        int numSpaces;
    };
}
