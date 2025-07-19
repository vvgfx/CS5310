#ifndef __PNG_IMAGELOADER_H_
#define __PNG_IMAGELOADER_H_

#include "ImageLoader.h"
#include <stdexcept>
#include <iostream>

// Using stb_image
// Downloaded from: https://github.com/nothings/stb
#include "stb_image.h"

namespace sgraph
{
    /**
     * @brief This class is used to load an image in the PNG format using stb_image
     * 
     * Features:
     * - Flips image vertically to match OpenGL texture coordinate system
     * - Converts all images to RGB format for consistency with PPM loader
     * - HDR images are automatically loaded as float* instead of GLubyte
     */
    class STBImageLoader: public ImageLoader {
   
        public:
            STBImageLoader(bool flip = true) {
                //vertical flip to match OpenGL's bottom-left origin
                stbi_set_flip_vertically_on_load(flip);
            }
   
            void load(string filename) {
                int channels;
                bool useFloatImage = false;
                if(filename.find(".hdr") != string::npos)
                    useFloatImage = true;
                float* loadedFloatImage;
                GLubyte* loaded_image;

                if(useFloatImage)
                    loadedFloatImage = stbi_loadf(filename.c_str(), &width, &height, &channels, 0); // keep original channel count
                else
                    loaded_image = stbi_load(filename.c_str(), &width, &height, &channels, 3);
                
                if (!loaded_image && !loadedFloatImage) {
                    throw std::invalid_argument("Failed to load image: " + string(stbi_failure_reason()));
                }
                
                std::cout << "Image file loaded: " << filename << std::endl;
                std::cout << "Dimensions: " << width << "x" << height << std::endl;
                
                if(useFloatImage)
                {
                    floatImage = new float[3 * width * height];
                    for (int i = 0; i < 3 * width * height; i++) {
                        floatImage[i] = loadedFloatImage[i];
                    }
                    stbi_image_free(loadedFloatImage);
                }
                else
                {
                    image = new GLubyte[3 * width * height];
                    for (int i = 0; i < 3 * width * height; i++) {
                        image[i] = loaded_image[i];
                    }
                    stbi_image_free(loaded_image);
                }
            }
            
            /**
             * @brief Alternative load method that preserves original channel count
             */
            void loadWithAlpha(string filename) {
                int channels;
                bool useFloatImage = false;
                if(filename.find(".hdr") != string::npos)
                    useFloatImage = true;
                float* loadedFloatImage;
                GLubyte* loaded_image;

                if(useFloatImage)
                    loadedFloatImage = stbi_loadf(filename.c_str(), &width, &height, &channels, 0); // keep original channel count always
                else
                    loaded_image = stbi_load(filename.c_str(), &width, &height, &channels, 0); // keep 0 here as well to preserve alpha
                
                if (!loaded_image && !loadedFloatImage) {
                    throw std::invalid_argument("Failed to load image: " + string(stbi_failure_reason()));
                }
                
                std::cout << "Image file loaded: " << filename << std::endl;
                std::cout << "Dimensions: " << width << "x" << height << std::endl;
                
                if(useFloatImage)
                {
                    floatImage = new float[3 * width * height];
                    for (int i = 0; i < 3 * width * height; i++) {
                        floatImage[i] = loadedFloatImage[i];
                    }
                    stbi_image_free(loadedFloatImage);
                }
                else
                {
                    image = new GLubyte[3 * width * height];
                    for (int i = 0; i < 3 * width * height; i++) {
                        image[i] = loaded_image[i];
                    }
                    stbi_image_free(loaded_image);
                }
                
                
                
                
            }
            
        private:
            int channels = 3; // Default to RGB
    };
}

#endif