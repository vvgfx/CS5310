#ifndef _SGNODE_H_
#define _SGNODE_H_

#include <glm/glm.hpp>
#include <vector>
#include <stack>
#include <string>
#include "Light.h"
using namespace std;

namespace sgraph {
  class IScenegraph;
  class SGNodeVisitor;

  /**
 * This interface represents all the operations offered by each type of node in our scenegraph.
 * Individual nodes may contain other functions unique to them.
 * Not all types of nodes are able to offer all types of operations.
 *
 * \author Amit Shesh
 */
  class SGNode {
  public:
    /**
     * In the scene graph rooted at this node, get the node whose name is as given
     * \param name name of node to be searched
     * \return the node reference if it exists, null otherwise
     */
    virtual SGNode *getNode(const string& name)=0;
    SGNode(){}

    virtual ~SGNode(){}

    
    /**
     * Return a deep copy of the scene graph subtree rooted at this node
     * \return a reference to the root of the copied subtree
     */
    virtual SGNode *clone()=0;

    /**
     * Set the parent of this node. Each node except the root has a parent
     * \param parent the node that is to be the parent of this node
     */
    virtual void setParent(SGNode *parent)=0;


    virtual SGNode* getParent()=0;

    /**
     * Traverse the scene graph rooted at this node, and store references to the scenegraph object
     * \param graph a reference to the scenegraph object of which this tree is a part
     */

    virtual void setScenegraph(sgraph::IScenegraph *graph)=0;

    /**
     * Set the name of this node. The name is not guaranteed to be unique in the tree, but it should be.
     * \param name the name of this node
     */
    virtual void setName(const string& name)=0;


    /**
     * Get the name of this node
     * \return the name of this node
     */
    virtual string getName()=0;

    /**
     * Accept a visitor to visit this node
     * 
     */
    
    virtual void accept(SGNodeVisitor *visitor)=0;


    // Adding defenitions for light stuff here. Currently, they have been defined in AbstractSGNode

    /**
     * Add a light to this node.
     */
    virtual void addLight(util::Light& light)=0;

    /**
     * Set all the lights for this node.
     */
    virtual void setLight(vector<util::Light>& lights)=0;

    /**
     * Get all the lights attached to this node.
     */
    virtual vector<util::Light>* getLights()=0;

    protected:
    /**
     * The name given to this node
     * 
     * Note: Moving this to the parent because I expect all Nodes to have a name now.
     */
    string name;
};
}

#endif
