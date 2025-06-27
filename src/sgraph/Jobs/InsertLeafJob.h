#ifndef _INSERTLEAFJOB_H_
#define _INSERTLEAFJOB_H_

#include <string>
#include "../../Model.h"
#include "../Commands/InsertLeafCommand.h"
#include "Ijob.h"
using namespace std;

namespace job
{

    /**
     * This is an implementation of the IJob interface.
     * It uses the command design pattern to insert a leaf node under the given node.
     *
     * Note: This is a part of the controller.
     */
    class InsertLeafJob : public IJob
    {
    public:
        InsertLeafJob(string nodeName, string newNodeName, glm::vec3 albedoVal, float metallicVal, float roughnessVal, float aoVal, string instanceOf, string texName)
        {
            this->albedo = albedoVal;
            this->nodeName = nodeName;
            this->newNodeName = newNodeName;
            this->metallic = metallicVal;
            this->roughness = roughnessVal;
            this->ao = aoVal;
            this->instanceOf = instanceOf;
            this->textureName = texName;
        }

        virtual void execute(Model *m)
        {
            util::Material material;
            material.setAlbedo(albedo.x, albedo.y, albedo.z);
            material.setRoughness(roughness);
            material.setMetallic(metallic);
            material.setAO(ao);

            command::InsertLeafCommand* leafCommand = new command::InsertLeafCommand(nodeName, newNodeName, m->getScenegraph(), material, instanceOf, textureName);
            cout<<"Adding to command queue in job"<<endl;
            m->addToCommandQueue(leafCommand);
        }

    private:
        string newNodeName;
        glm::vec3 albedo;
        float metallic, roughness, ao;
        string instanceOf, textureName;
    };
}

#endif