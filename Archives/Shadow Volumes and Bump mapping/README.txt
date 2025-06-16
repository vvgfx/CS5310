This program implements a stencil shadow volume demo.

It uses multiple render passes, with blending shadows to support multiple lights.

Features:
- Hierarchial Scenegraphs : The 3d world is saved in a tree data structure.(Scenegraph)
- Text parser : Make changes/build a new world by writing a scenegraph in text. (Look at scenegraphmodels folder for examples.)
- Obj loader : This program can load Obj models. (saves them with adjacency data)
- World objects: Supports the following features:
    - Meshed objects: any obj format object is supported. This program imports only the position, normal and texture coordinate data.
    - Lights : Point and spot lights
    - Shadows : Resolution independent shadows using stencil shadow volumes.
    - Textures : Support for textures with mip-mapping. The program can import only PPM P3 Images.

Note:
- By default, the program runs the "test.txt" scene.

Requirements :
- Platform that can run OpenGL 3.3
- C++ compiler
- Make utility


All textures used in the sample scene are from https://freestylized.com . They have a royalty free license for all commercial and non-commercial purposes. They were converted to PPM P3 using GIMP (with ASCII export option.)
stylized wall: https://freestylized.com/material/stones_bricks_wall_01/
