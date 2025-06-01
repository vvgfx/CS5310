#ifndef _LEAFNODE_H_
#define _LEAFNODE_H_

#include "AbstractSGNode.h"
#include "SGNodeVisitor.h"
#include "Material.h"
#include "glm/glm.hpp"
#include <map>
#include <stack>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;

namespace sgraph
{

/**
 * This node represents the leaf of a scene graph. It is the only type of node that has
 * actual geometry to render.
 * \author Amit Shesh
 */
class LeafNode: public AbstractSGNode
{
    /**
     * The name of the object instance that this leaf contains. All object instances are stored
     * in the scene graph itself, so that an instance can be reused in several leaves
     */
protected:
    string objInstanceName;
    /**
     * The material associated with the object instance at this leaf
     */
    util::Material material;
    string textureName;
    glm::mat4 textureTransform;
    string normalTextureName;
    bool isBumpMapping;

public:
    LeafNode(const string& instanceOf,util::Material& material,const string& name,sgraph::IScenegraph *graph, string texName, string normalName)
        :AbstractSGNode(name,graph) {
        this->objInstanceName = instanceOf;
        this->material = material;
        this->textureName = texName;
        this->normalTextureName = normalName;
        textureTransform  = glm::mat4(1.0f);//Assuming that the default texture transformation is the identity matrix.
    }

    LeafNode(const string& instanceOf,const string& name,sgraph::IScenegraph *graph, string texName, string normalName)
        :AbstractSGNode(name,graph) {
        this->objInstanceName = instanceOf;
        this->textureName = texName;
        this->normalTextureName = normalName;
        this->isBumpMapping = false;
        // this->textureTransform = glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), glm::vec3(5.0f, 5.0f, 5.0f)); // Clean build before using!!
        this->textureTransform = glm::mat4(1.0f);
    }
	
	~LeafNode(){}



    /*
	 *Set the material of each vertex in this object
	 */
    void setMaterial(const util::Material& mat) {
        material = mat;
    }

    /*
     * gets the material
     */
    util::Material getMaterial()
    {
        return material;
    }

    /**
     * Set the name of the texture corresponding to this leaf.
     */
    void setTextureName(string texName)
    {
        textureName = texName;
    }

    /**
     * Get the nameof the texture corresponding to this leaf.
     */
    string getTextureName()
    {
        return textureName;
    }


    /**
     * Set the name of the normal texture corresponding to this leaf.
     */
    void setNormalTextureName(string texName)
    {
        this->isBumpMapping = true;
        cout<<"bump mapping set to true"<<endl;
        normalTextureName = texName;
    }


    /**
     * get if bump mapping is used. Temporary testing code.
     */
    bool getBumpMappingBool()
    {
        return this->isBumpMapping;
    }

    /**
     * Get the nameof the normal texture corresponding to this leaf.
     */
    string getNormalTextureName()
    {
        return normalTextureName;
    }

    /**
     * Get the texture transform matrix of this leaf.
     */
    glm::mat4 getTextureTransform()
    {
        return this->textureTransform;
    }

    /**
     * Set the texture transform matrix of this leaf.
     */
    void setTextureTransform(glm::mat4 texTransform)
    {
        this->textureTransform = texTransform;
    }

    /**
     * Get the name of the instance this leaf contains
     * 
     * @return string 
     */
    string getInstanceOf() {
        return this->objInstanceName;
    }

    /**
     * Get a copy of this node.
     * 
     * @return SGNode* 
     */

    SGNode *clone() {
        LeafNode *newclone = new LeafNode(this->objInstanceName,material,name,scenegraph, this->textureName, this->normalTextureName);
        return newclone;
    }

    /**
     * Visit this node.
     * 
     */
    void accept(SGNodeVisitor* visitor) {
      visitor->visitLeafNode(this);
    }
};
}
#endif
