This program implements a Rendering engine.

It uses multiple render passes, with blending shadows to support multiple lights.

Features: (This is outdated, refer to the main readme!)
- Hierarchial Scenegraphs : The 3d world is saved in a tree data structure.(Scenegraph)
- Text parser : Make changes/build a new world by writing a scenegraph in text. (Look at scenegraphmodels folder for examples.)
- Obj loader : This program can load Obj models. (saves them with adjacency data)
- World objects: Supports the following features:
    - Meshed objects: any obj format object is supported. This program imports only the position, normal and texture coordinate data.
    - Lights : Point and spot lights
    - Shadows : Resolution independent shadows using stencil shadow volumes.
    - Textures : Support for textures with mip-mapping. The program can import only PPM P3 Images.

Note:
- To load a new scenegraph txt file, either use the load option in the GUI or pass the file name as an argument to the executable.

Requirements :
- Platform that can run OpenGL 4.5
- C++ compiler
- Make utility