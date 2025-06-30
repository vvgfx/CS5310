#ifndef _UPDATELEAFMATERIALCOMMAND_H_
#define _UPDATELEAFMATERIALCOMMAND_H_

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
#include <algorithm>
using namespace std;

namespace command
{
    /**
     * This command uses the command design pattern to make updates to a scenegraph's nodes
     *
     */
    class UpdateLeafMaterialCommand : public AbstractCommand
    {
    public:
        /**
         * @brief Construct a new UpdateLeafMaterialCommand object
         *
         * @param name Name of the node this command should effect
         * 
         */
        UpdateLeafMaterialCommand(string name, glm::vec4 albedo, float metallic, float roughness, float ao)
        {
            this->nodeName = name;
            this->albedo = albedo;
            this->metallic = metallic;
            this->roughness = roughness;
            this->ao = ao;
        }

        /**
         * @brief Override for updating
         *
         *
         * @param leafNode
         */
        void visitLeafNode(sgraph::LeafNode *leafNode)
        {
            util::Material newMat;
            newMat.setAlbedo(albedo);
            newMat.setMetallic(metallic);
            newMat.setRoughness(roughness);
            newMat.setAO(ao);
            leafNode->setMaterial(newMat);
        }
        
    private:
        glm::vec4 albedo;
        float metallic, roughness, ao;
    };
}

#endif