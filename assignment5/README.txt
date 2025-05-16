This program implements a simple scene graph, and renders it.

A scene graph has various kinds of nodes:

1. Group node: a logical grouping of other nodes in the graph. It can have zero or more children.

2. Leaf node: this is the only node that contains drawable geometry. It remembers an instance of the object to be drawn, so that the same mesh is not copied over in multiple leaves.

3. Transform node: this represents a single transformation. It has a single child. There are three kinds of specific transformations:
    a. Scale transform
    b. Translate transform
    c. Rotate transform
    

The Scenegraph class implements a scene graph (specifically the IScenegraph abstract class).

To implement operations on a scene graph, a visitor pattern is used. The NodeVisitor abstract class represents the interface of a visitor. An example implementation is provided in the GLScenegraphRenderer class, which renders the scene graph using OpenGL.

Finally, a command-language is implemented that makes it convenient to specify a scene graph. Some commands it supports are:

a. translate variable-name node-name tx ty tz: create a new translate transform node with name "node-name". variable-name is used to refer to this node in the command language

b. group variable-name node-name: self-explanatory

c. add-child abc def: Add the node whose variable-name is "abc" as a child of the node whose variable-name is "def"

d. assign-material leaf material: Assign to the leaf with variable-name "leaf" the material with variable-name "material"

and so on.

Note that single-line comments are allowed in the command language: a line that begins with # is a comment.


Several examples of scene graphs have been given, in the scenegraphmodels folder: 

1. simple.txt: shows a simple scene graph with one box with transformations applied.

2. face-hierarchy-commands.txt: shows a scene graph that creates a clown face with a hat. In comments, the scene graph that is progressively built is shown, for illustration.

3. humanoid-commands.txt: shows a scene graph that creates a humanoid stick figure. Look for comments IN CAPS in the "right arm" part of this file, for an example of how to manipulate this scene graph to change the pose of the humanoid, if need be.

4. face-hierarchy-with-copy-commands.txt: shows an example of a command that allows one to create a copy of part of a scene graph. See towards the end of this file.

5. two-humanoids.txt: shows an example of how one command can be used to import an entire scene graph from another command file. This is helpful when creating a more complicated scene. Instead of writing all the commands in one file, one can create smaller pieces in separate files and then create a "main commands file" that imports them.


Further additions:

1. Support for a 3rd type of node: Dynamic Transform -  This is to make runtime modifications to the scenegraph easier.
2. Support for lighting - extended the xml parser to support new commands "light" and "assign-light", then factored changes for supporting the same in the scenegraph and view.
3. Support for textures - extended the xml parser ot support new commands "image" and "assign-texture", then factored changes for supporting the same in the scenegraph and view. Note: only PPM P3 textures are supported currently.
4. Sample scene - big-ben.txt

All textures used in the sample scene are from https://freestylized.com . They have a royalty free license for all commercial and non-commercial purposes. THey were converted to PPM P3 using GIMP (with ASCII export option.)
Stylized grass : https://freestylized.com/material/grass_01/
Stylized walls : https://freestylized.com/material/bricks_wall_11/, https://freestylized.com/material/stones_wall_03/
Stylized roof : 
