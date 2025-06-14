## This Repo contains a rendering engine.

It has the following features:
- Text parser - 3d worlds can be built by creating a text file and passing it as an argument to the program. Sample text files are provided in the scenegraphmodels folder
- Hierarchial scenegraphs - Tree representation of the 3d world. This is by parsing text files. These files can be passed as an argument to the program.
- Dynamic transformation - Simple transformations at runtime such as rotations, translations and scales can be done wrt time and some samples are provided in the included text file.
- Physically based rendering - PBR workflow based on Epic Game's "Real Shading in Unreal Engine 4" SIGGRAPH paper.


Each pipeline contains different features I've implemented on top of the base software renderer. Some of them are:
- Shadow volumes - Each object casts shadows. This is done by stencil shadow volumes with blending between different light sources.
- Normal maps - Support for normal maps using bump-mapping.

### To-do list:

- Importance sampling for image based lighting.
- MSAA.
- Deferred rendering / clustered forward rendering.

(This is not exhaustive)

### Requirements:

- OpenGL 3.3+
- C++ compiler (g++)
- Make utility


### Author's blog post
https://vv-22.github.io/rendering-with-opengl



### References:

- https://ogldev.org/www/tutorial40/tutorial40.html  - Used this reference for shadow volumes.
- https://freestylized.com - All textures are from this website. 



### Acknowledgements:

A huge thank you to my professor, Amit Shesh, for all his guidance.

Thank you, to the graphics community for paving the way and providing me with the motivation to continue building this.

Shoutout to https://freestylized.com , for providing me with the textures to make something beautiful.
