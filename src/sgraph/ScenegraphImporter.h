#ifndef _SCENEGRAPHIMPORTER_H_
#define _SCENEGRAPHIMPORTER_H_

#include "IScenegraph.h"
#include "Scenegraph.h"
#include "GroupNode.h"
#include "LeafNode.h"
#include "TransformNode.h"
#include "RotateTransform.h"
#include "ScaleTransform.h"
#include "TranslateTransform.h"
#include "DynamicTransform.h"
#include "PPMImageLoader.h"
#include "PolygonMesh.h"
#include "Material.h"
#include "Light.h"
#include "TextureImage.h"
#include "ObjAdjImporter.h"
#include <istream>
#include <map>
#include <string>
#include <iostream>
using namespace std;

namespace sgraph {
    class ScenegraphImporter {
        public:
            ScenegraphImporter(string defaultTexPath) 
            {
                PPMImageLoader textureLoader;
                textureLoader.load(defaultTexPath);
                util::TextureImage* texImage = new util::TextureImage(textureLoader.getPixels(), textureLoader.getWidth(), textureLoader.getHeight(), "default"); // directly converting to reference. Hope this works.
                textureMap["default"] = texImage;
            }

            IScenegraph *parse(istream& input) {
                string command;
                string inputWithOutCommentsString = stripComments(input);
                istringstream inputWithOutComments(inputWithOutCommentsString);
                while (inputWithOutComments >> command) {
                    cout << "Read " << command << endl;
                    if (command == "instance") {
                        string name,path;
                        inputWithOutComments >> name >> path;
                        cout << "Read " << name << " " << path << endl;
                        meshPaths[name] = path;
                        ifstream in(path);
                       if (in.is_open()) {
                        util::PolygonMesh<VertexAttrib> mesh = util::ObjAdjImporter<VertexAttrib>::importFile(in,false);
                        meshes[name] = mesh;         
                       } 
                    }
                    else if (command == "light")
                    {
                        parseLight(inputWithOutComments);
                    }
                    else if (command == "assign-light")
                    {
                        parseAssignLight(inputWithOutComments);
                    }
                    else if (command == "dynamic")
                    {
                        parseDynamic(inputWithOutComments);
                    }
                    else if (command == "image")
                    {
                        parseTexture(inputWithOutComments);
                    }
                    else if (command == "assign-texture")
                    {
                        parseAssignTexture(inputWithOutComments);
                    }
                    else if (command == "assign-normal")
                    {
                        parseAssignNormal(inputWithOutComments);
                    }
                    else if (command == "assign-metallic")
                    {
                        parseAssignMetallic(inputWithOutComments);
                    }
                    else if (command == "assign-roughness")
                    {
                        parseAssignRoughness(inputWithOutComments);
                    }
                    else if (command == "assign-ao")
                    {
                        parseAssignAO(inputWithOutComments);
                    }
                    else if (command == "group") {
                        parseGroup(inputWithOutComments);
                    }
                    else if (command == "leaf") {
                        parseLeaf(inputWithOutComments);
                    }
                    else if (command == "material") {
                        parseMaterial(inputWithOutComments);
                    }
                    else if (command == "scale") {
                        parseScale(inputWithOutComments);
                    }
                    else if (command == "rotate") {
                        parseRotate(inputWithOutComments);
                    }
                    else if (command == "translate") {
                        parseTranslate(inputWithOutComments);
                    }
                    else if (command == "copy") {
                        parseCopy(inputWithOutComments);
                    }
                    else if (command == "import") {
                        parseImport(inputWithOutComments);
                    }
                    else if (command == "assign-material") {
                        parseAssignMaterial(inputWithOutComments);
                    }
                    else if (command == "add-child") {
                        parseAddChild(inputWithOutComments);
                    }
                    else if (command == "assign-root") {
                        parseSetRoot(inputWithOutComments);
                    }
                    else {
                        throw runtime_error("Unrecognized or out-of-place command: "+command);
                    }
                }
                if (root!=NULL) {
                    IScenegraph *scenegraph = new Scenegraph();
                    scenegraph->makeScenegraph(root);
                    scenegraph->setMeshes(meshes);
                    scenegraph->setMeshPaths(meshPaths);
                    return scenegraph;
                }
                else {
                    throw runtime_error("Parsed scene graph, but nothing set as root");
                }
            }

            map<string, util::TextureImage*> getTextureMap()
            {
                return this->textureMap;
            }


            map<string, SGNode*> getNodeMap()
            {
                return this->nodes;
            }

            // map<string, util::TextureImage*> getNormalMap()
            // {
            //     return this->normalMap;
            // }
            protected:

                virtual void parseTexture(istream& input)
                {
                    string texName, texPath;
                    input >> texName >> texPath;
                    cout << "Read " << texName << " " << texPath << endl;
                    PPMImageLoader textureLoader;
                    textureLoader.load(texPath);
                    util::TextureImage* texImage = new util::TextureImage(textureLoader.getPixels(), textureLoader.getWidth(), textureLoader.getHeight(), texName); // directly converting to reference. Hope this works.
                    textureMap[texName] = texImage;
                }

                virtual void parseAssignNormal(istream& input)
                {
                    string textureName, leafName;
                    input >> leafName >> textureName;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[leafName]);
                    if ((leafNode != nullptr) && (textureMap.find(textureName) != textureMap.end())) 
                        leafNode->setNormalMap(textureName);
                }

                virtual void parseAssignMetallic(istream& input)
                {
                    string textureName, leafName;
                    input >> leafName >> textureName;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[leafName]);
                    if ((leafNode != nullptr) && (textureMap.find(textureName) != textureMap.end())) 
                        leafNode->setMetallicMap(textureName);
                }
                virtual void parseAssignRoughness(istream& input)
                {
                    string textureName, leafName;
                    input >> leafName >> textureName;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[leafName]);
                    if ((leafNode != nullptr) && (textureMap.find(textureName) != textureMap.end())) 
                        leafNode->setRoughnessMap(textureName);
                }
                virtual void parseAssignAO(istream& input)
                {
                    string textureName, leafName;
                    input >> leafName >> textureName;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[leafName]);
                    if ((leafNode != nullptr) && (textureMap.find(textureName) != textureMap.end())) 
                        leafNode->setAOMap(textureName);
                }

                virtual void parseAssignTexture(istream& input)
                {
                    string textureName, leafName;
                    input >> leafName >> textureName;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[leafName]);
                    if ((leafNode != nullptr) && (textureMap.find(textureName) != textureMap.end())) 
                        leafNode->setTextureMap(textureName);
                }

                virtual void parseDynamic(istream& input)
                {
                    string varname, name;
                    input >> varname >> name;
                    cout << "Read " << varname << " " << name << endl;
                    SGNode *dynamic = new DynamicTransform(glm::mat4(1.0), name, NULL);
                    nodes[varname] = dynamic;
                }

                virtual void parseGroup(istream& input) {
                    string varname,name;
                    input >> varname >> name;
                    
                    cout << "Read " << varname << " " << name << endl;
                    SGNode *group = new GroupNode(name,NULL);
                    nodes[varname] = group;
                }

                virtual void parseLeaf(istream& input) {
                    string varname,name,command,instanceof;
                    input >> varname >> name;
                    cout << "Read " << varname << " " << name << endl;
                    input >> command;
                    if (command == "instanceof") {
                        input >> instanceof;
                    }
                    SGNode *leaf = new LeafNode(instanceof,name,NULL, "default"); // changed this to remove "default" and "default-normal" hardcode from leafNode.
                    LeafNode* leafInstance = dynamic_cast<LeafNode*>(leaf);
                    nodes[varname] = leaf;
                } 

                virtual void parseScale(istream& input) {
                    string varname,name;
                    input >> varname >> name;
                    float sx,sy,sz;
                    input >> sx >> sy >> sz;
                    SGNode *scaleNode = new ScaleTransform(sx,sy,sz,name,NULL);
                    nodes[varname] = scaleNode;
                }

                virtual void parseTranslate(istream& input) {
                    string varname,name;
                    input >> varname >> name;
                    float tx,ty,tz;
                    input >> tx >> ty >> tz;
                    SGNode *translateNode = new TranslateTransform(tx,ty,tz,name,NULL);
                    nodes[varname] = translateNode;         
                }

                virtual void parseRotate(istream& input) {
                    string varname,name;
                    input >> varname >> name;
                    float angleInDegrees,ax,ay,az;
                    input >> angleInDegrees >> ax >> ay >> az;
                    SGNode *rotateNode = new RotateTransform(glm::radians(angleInDegrees),ax,ay,az,name,NULL);
                    nodes[varname] = rotateNode;         
                }

                virtual void parseMaterial(istream& input) {
                    util::Material mat;
                    float r,g,b;
                    string name;
                    input >> name;
                    string command;
                    input >> command;
                    while (command!="end-material") {
                        if (command == "ambient") {
                            input >> r >> g >> b;
                            mat.setAmbient(r,g,b);
                        }
                        else if (command == "diffuse") {
                            input >> r >> g >> b;
                            mat.setDiffuse(r,g,b);
                        }
                        else if (command == "specular") {
                            input >> r >> g >> b;
                            mat.setSpecular(r,g,b);
                        }
                        else if (command == "emission") {
                            input >> r >> g >> b;
                            mat.setEmission(r,g,b);
                        }
                        else if (command == "shininess") {
                            input >> r;
                            mat.setShininess(r);
                        }
                        // setting up PBR stuff here!
                        else if (command == "albedo") {
                            input >> r >> g >> b;
                            mat.setAlbedo(r,g,b);
                        }
                        else if (command == "metallic") {
                            input >> r;
                            mat.setMetallic(r);
                        }
                        else if (command == "roughness") {
                            input >> r;
                            mat.setRoughness(r);
                        }
                        else if (command == "ao") {
                            input >> r;
                            mat.setAO(r);
                        }
                        else
                            throw runtime_error("Material property is not recognized : " + command);
                        input >> command;
                    }
                    materials[name] = mat;
                }

                virtual void parseLight(istream& input)
                {
                    util::Light light;
                    float r, g, b;
                    float x, y, z;
                    string name;
                    input >> name;
                    string command;
                    input >> command;
                    while(command != "end-light")
                    {
                        if (command == "ambient")
                        {
                            input >> r >> g >> b;
                            light.setAmbient(r, g, b);
                        }
                        else if (command == "diffuse")
                        {
                            input >> r >> g >> b;
                            light.setDiffuse(r, g, b);
                        }
                        else if (command == "specular")
                        {
                            input >> r >> g >> b;
                            light.setSpecular(r, g, b);
                        }
                        else if (command == "position")
                        {
                            input >> x >> y >> z;
                            light.setPosition(x, y, z);
                        }
                        else if (command == "spot-direction")
                        {
                            input >> x >> y >> z;
                            light.setSpotDirection(x, y, z);
                        }
                        else if (command == "spot-angle")
                        {
                            input >> x;
                            light.setSpotAngle(x);
                        }
                        // PBR color here
                        else if (command == "color")
                        {
                            input >> r >> g >> b;
                            light.setColor(r, g, b);
                        }
                        else
                            throw runtime_error("Light property is not recognized : " + command);
                        input >> command;
                    }
                    lights[name] = light;
                }

                virtual void parseAssignLight(istream& input)
                {
                    string lightName, parentName;
                    input >> lightName >> parentName;

                    SGNode* node = nodes[parentName]; 
                    if ((node != nullptr) && (lights.find(lightName) != lights.end())) 
                    {
                        node->addLight(lights[lightName]);
                    }

                }

                virtual void parseCopy(istream& input) {
                    string nodename,copyof;

                    input >> nodename >> copyof;
                    if (nodes.find(copyof)!=nodes.end()) {
                        SGNode * copy = nodes[copyof]->clone();
                        nodes[nodename] = copy;
                    }
                }

                virtual void parseImport(istream& input) {
                    string nodename,filepath;

                    input >> nodename >> filepath;
                    ifstream external_scenegraph_file(filepath);
                    if (external_scenegraph_file.is_open()) {
                        
                        IScenegraph *importedSG = parse(external_scenegraph_file);
                        nodes[nodename] = importedSG->getRoot();
                       /* for (map<string,util::PolygonMesh<VertexAttrib> >::iterator it=importedSG->getMeshes().begin();it!=importedSG->getMeshes().end();it++) {
                            this->meshes[it->first] = it->second;
                        }
                        for (map<string,string>::iterator it=importedSG->getMeshPaths().begin();it!=importedSG->getMeshPaths().end();it++) {
                            this->meshPaths[it->first] = it->second;
                        }
                        */
                        //delete the imported scene graph but not its nodes!
                        importedSG->makeScenegraph(NULL);
                        delete importedSG;
                    }
                }

                virtual void parseAssignMaterial(istream& input) {
                    string nodename,matname;
                    input >> nodename >> matname;

                    LeafNode *leafNode = dynamic_cast<LeafNode *>(nodes[nodename]);
                    if ((leafNode!=NULL) && (materials.find(matname)!=materials.end())) {
                        leafNode->setMaterial(materials[matname]);
                    }
                }

                virtual void parseAddChild(istream& input) {
                    string childname,parentname;

                    input >> childname >> parentname;
                    ParentSGNode * parentNode = dynamic_cast<ParentSGNode *>(nodes[parentname]);
                    SGNode * childNode = NULL;
                    if (nodes.find(childname)!=nodes.end()) {
                        childNode = nodes[childname];
                    }

                    if ((parentNode!=NULL) && (childNode!=NULL)) {
                        parentNode->addChild(childNode);
                    }
                }

                virtual void parseSetRoot(istream& input) {
                    string rootname;
                    input >> rootname;

                    root = nodes[rootname];

                    cout << "Root's name is "<< root->getName() << endl;
                }

                string stripComments(istream& input) {
                    string line;
                    stringstream clean;
                    while (getline(input,line)) {
                        int i=0;
                        while ((i<line.length()) && (line[i]!='#')) {
                            clean << line[i];
                            i++;
                        }
                        clean << endl;
                    }
                    return clean.str();
                }
            private:
                map<string,SGNode *> nodes;
                map<string,util::Material> materials;
                map<string,util::PolygonMesh<VertexAttrib> > meshes;
                map<string,string> meshPaths;
                SGNode *root;
                map<string, util::Light> lights;
                map<string,util::TextureImage*> textureMap;
                // map<string,util::TextureImage*> normalMap; // for bump mapping.
                //removed references to defaultTexturePath and defaultNormalPath because its not needed after the constructor :)
    };
}


#endif