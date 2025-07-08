#ifndef _SCENEGRAPHEXPORTER_H_
#define _SCENEGRAPHEXPORTER_H_

#include "SGNodeVisitor.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "DynamicTransform.h"
#include "SRTNode.h"
#include <sstream>
using namespace std;

namespace sgraph {
    class ScenegraphExporter: public SGNodeVisitor {
        public:
            ScenegraphExporter(map<string,string> meshPaths, map<string, string> texPaths, vector<string> cubeMapPaths) {
                level = 1;
                number = 0;

                append("# meshes");
                for (map<string,string>::iterator it=meshPaths.begin(); it!=meshPaths.end();it++) {
                    append("instance "+it->first + " " + it->second);
                }
                append("\n\n# textures");
                for (map<string,string>::iterator it=texPaths.begin(); it!=texPaths.end();it++) {
                    append("image "+it->first + " " + it->second);
                }
                append("\n\n");
                if(cubeMapPaths.size() > 0)
                {
                    append("# cubemap");
                    string valToAppend = "cubemap ";
                    for(string currPath : cubeMapPaths)
                        valToAppend+= (currPath + " ");
                    append(valToAppend);
                }
                append("\n\n");
            }

            string getOutput() {
                return output.str();
            }

            /**
             * @brief Recur to the children for drawing
             * 
             * @param groupNode 
             */
            void visitGroupNode(GroupNode *groupNode) {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = groupNode->getName();

                append("group " + varname + " " + groupNode->getName());
                appendLights(groupNode, varname);
                visitParentSGNode(groupNode,varname);

                if (level==1) {
                    append("assign-root "+varname);
                }
                
            }

            /**
             * @brief Draw the instance for the leaf, after passing the 
             * modelview and color to the shader
             * 
             * @param leafNode 
             */
            void visitLeafNode(LeafNode *leafNode) {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = leafNode->getName();

                appendLights(leafNode, varname);
                append("leaf " + varname + " " +leafNode->getName() + " " + "instanceof " +leafNode->getInstanceOf());                

                append("material mat-" + varname);
                stringstream mat;
                mat << "emission " 
                    << leafNode->getMaterial().getEmission()[0] << " "
                    << leafNode->getMaterial().getEmission()[1] << " "
                    << leafNode->getMaterial().getEmission()[2] << endl;
                mat << "ambient " 
                    << leafNode->getMaterial().getAmbient()[0] << " "
                    << leafNode->getMaterial().getAmbient()[1] << " "
                    << leafNode->getMaterial().getAmbient()[2] << endl;
                mat << "diffuse " 
                    << leafNode->getMaterial().getDiffuse()[0] << " "
                    << leafNode->getMaterial().getDiffuse()[1] << " "
                    << leafNode->getMaterial().getDiffuse()[2] << endl;
                mat << "specular " 
                    << leafNode->getMaterial().getSpecular()[0] << " "
                    << leafNode->getMaterial().getSpecular()[1] << " "
                    << leafNode->getMaterial().getSpecular()[2] << endl;
                mat << "shininess " << leafNode->getMaterial().getShininess() << endl;

                // PBR stuff here
                mat << "#PBR" << endl;
                mat << "albedo " 
                    << leafNode->getMaterial().getAlbedo()[0] << " "
                    << leafNode->getMaterial().getAlbedo()[1] << " "
                    << leafNode->getMaterial().getAlbedo()[2] << endl;
                mat << "metallic " 
                    << leafNode->getMaterial().getMetallic() << endl;
                mat << "roughness " 
                    << leafNode->getMaterial().getRoughness() << endl;
                mat << "ao " 
                    << leafNode->getMaterial().getAO();
                append(mat.str());
                append("end-material \n");
                append("assign-material "+varname+" mat-"+varname + " \n");

                // now do the same for textures
                string texName = leafNode->getTextureMap();
                if(texName != "")
                    append("assign-texture " + varname + " " + texName);
                    
                string normalMapName = leafNode->getNormalMap();
                if(normalMapName != "")
                    append("assign-normal " + varname + " " + normalMapName);

                string metallicMapName = leafNode->getMetallicMap();
                if(metallicMapName != "")
                    append("assign-metallic " + varname + " " + metallicMapName);

                string roughnessMap = leafNode->getRoughnessMap();
                if(roughnessMap != "")
                    append("assign-roughness " + varname + " " + roughnessMap);

                string aoMapName = leafNode->getAOMap();
                if(aoMapName != "")
                    append("assign-ao " + varname + " " + aoMapName);

                if (level==1) {
                    append("\n\n");
                    append("assign-root "+varname);
                }
            }

            /**
             * @brief Multiply the transform to the modelview and recur to child
             * 
             * @param transformNode 
             */
            void visitTransformNode(TransformNode * transformNode) {
                
            }

            /**
             * @brief For this visitor, only the transformation matrix is required.
             * Thus there is nothing special to be done for each type of transformation.
             * We delegate to visitTransformNode above
             * 
             * @param scaleNode 
             */
            void visitScaleTransform(ScaleTransform *scaleNode) {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = scaleNode->getName();

                stringstream t;
                t << "scale " << varname  << " " << scaleNode->getName() + " "
                  << scaleNode->getScale()[0] << " "
                  << scaleNode->getScale()[1] << " "
                  << scaleNode->getScale()[2] << "\n";            

                append(t.str());

                appendLights(scaleNode, varname);
                visitParentSGNode(scaleNode,varname);

                if (level==1) {
                    append("\n\n");
                    append("assign-root "+varname);
                }
                
            }

            /**
             * @brief For this visitor, only the transformation matrix is required.
             * Thus there is nothing special to be done for each type of transformation.
             * We delegate to visitTransformNode above
             * 
             * @param translateNode 
             */
            void visitTranslateTransform(TranslateTransform *translateNode) {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = translateNode->getName();

                stringstream t;
                t << "translate " << varname << " " + translateNode->getName()
                  << translateNode->getTranslate()[0] << " "
                  << translateNode->getTranslate()[1] << " "
                  << translateNode->getTranslate()[2] << "\n";            

                append(t.str());
                appendLights(translateNode, varname);
                visitParentSGNode(translateNode,varname);

                if (level==1) {
                    append("\n\n");
                    append("assign-root "+varname);
                }
            }

            void visitRotateTransform(RotateTransform *rotateNode) {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = rotateNode->getName();
                
                stringstream t;
                t << "rotate " << varname << " " +rotateNode->getName()
                  << glm::degrees(rotateNode->getAngleInRadians()) << " "
                  << rotateNode->getRotationAxis()[0] << " "
                  << rotateNode->getRotationAxis()[1] << " "
                  << rotateNode->getRotationAxis()[2] << "\n";

                append(t.str());
                appendLights(rotateNode, varname);
                visitParentSGNode(rotateNode,varname);

                if (level==1) {
                    append("\n\n");
                    append("assign-root "+varname);
                }
            }

            void visitDynamicTransform(DynamicTransform* dynamicTransformNode)
            {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = dynamicTransformNode->getName();
                stringstream t;


                t << "dynamic " << varname << " " << dynamicTransformNode->getName() << "\n";
                append(t.str());
                appendLights(dynamicTransformNode, varname);
                visitParentSGNode(dynamicTransformNode,varname);

                if (level==1) {
                    append("\n\n");
                    append("assign-root "+varname);
                }
            }

            void visitSRTNode(SRTNode* srtNode)
            {
                string varname;
                stringstream namestream;
                namestream << "node-" << level << "-" << number;
                varname = srtNode->getName();


                stringstream t;
                t << "srt " << varname << " " << srtNode->getName() << " "
                    << srtNode->getScale().x << " " << srtNode->getScale().y << " " << srtNode->getScale().z << " "
                    << glm::degrees(srtNode->getRotate().x) << " " << glm::degrees(srtNode->getRotate().y) << " " << glm::degrees(srtNode->getRotate().z) << " "
                    << srtNode->getTranslate().x << " " << srtNode->getTranslate().y << " " << srtNode->getTranslate().z << "\n";
                    
                append(t.str());
                appendLights(srtNode, varname);
                visitParentSGNode(srtNode,varname);

                if (level==1) {
                    append("\n\n");
                    append("assign-root "+varname);
                }
            }

        private:
            void append(const string& str) {                
                output << str << endl;
            }

            void visitParentSGNode(ParentSGNode * node,const string& name) {
                level +=1;
                int old = number;
                number = 0;

                for (int i=0;i<node->getChildren().size();i=i+1) {
                    node->getChildren()[i]->accept(this);
                    stringstream childname;
                    childname << "node-" << level << "-" << number;
                    append("add-child " + node->getChildren()[i]->getName() + " " +name);
                    number = number + 1;
                }
                level -=1;
                number = old;
            }

            void appendLights(SGNode* node, const string& name)
            {
                vector<util::Light>* lights = node->getLights();

                for(vector<util::Light>::iterator it = lights->begin(); it != lights->end(); it++)
                {
                    append("light " + it->getName());
                    stringstream l;
                    l << "ambient " << it->getAmbient().x << " " << it->getAmbient().y << " " << it->getAmbient().z << endl;
                    l << "diffuse " << it->getDiffuse().x << " " << it->getDiffuse().y << " " << it->getDiffuse().z << endl;
                    l << "specular " << it->getSpecular().x << " " << it->getSpecular().y << " " << it->getSpecular().z << endl;
                    l << "position " << it->getPosition().x << " " << it->getPosition().y << " " << it->getPosition().z << endl;
                    if(it->getSpotCutoff() > 0)
                    {
                        l << "spot-direction " << it->getSpotDirection().x << " " << it->getSpotDirection().y << " " << it->getSpotDirection().z << endl;
                        l << "spot-angle " << it->getSpotCutoff() << endl;
                    }
                    l << "color " << it->getColor().x << " " << it->getColor().y << " " << it->getColor().z;
                    append(l.str());
                    append("end-light");
                    append("assign-light " + it->getName() + " " + name + "\n");
                }

            }
            int level; //the level of the scene graph
            int number; //the number of the current node at the current level (only changes with parent sg nodes)
            stringstream output;

    };
}


#endif