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
     */
    class PNGImageLoader: public ImageLoader {
   
        public:
            PNGImageLoader() {
                //vertical flip to match OpenGL's bottom-left origin
                // stbi_set_flip_vertically_on_load(true); // commenting this out for now because the cubemaps look inverted
            }
   
            void load(string filename) {
                int channels;
                
                GLubyte* loaded_image = stbi_load(filename.c_str(), &width, &height, &channels, 3);
                
                if (!loaded_image) {
                    throw std::invalid_argument("Failed to load image: " + string(stbi_failure_reason()));
                }
                
                std::cout << "Image file loaded: " << filename << std::endl;
                std::cout << "Dimensions: " << width << "x" << height << std::endl;
                
                image = new GLubyte[3 * width * height];
                
                for (int i = 0; i < 3 * width * height; i++) {
                    image[i] = loaded_image[i];
                }
                
                stbi_image_free(loaded_image);
            }
            
            /**
             * @brief Alternative load method that preserves original channel count
             */
            void loadWithAlpha(string filename) {
                int channels;
                
                // Load image with original channel count
                GLubyte* loaded_image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
                
                if (!loaded_image) {
                    throw std::invalid_argument("Failed to load image: " + string(stbi_failure_reason()));
                }
                
                std::cout << "Image file loaded: " << filename << std::endl;
                std::cout << "Dimensions: " << width << "x" << height << std::endl;
                
                image = new GLubyte[channels * width * height];
                
                for (int i = 0; i < channels * width * height; i++) {
                    image[i] = loaded_image[i];
                }
                
                this->channels = channels;
                
                stbi_image_free(loaded_image);
            }
            
        private:
            int channels = 3; // Default to RGB
    };
}

#endif