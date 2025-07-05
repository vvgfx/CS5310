#ifndef __PNG_IMAGELOADER_H_
#define __PNG_IMAGELOADER_H_

#include "ImageLoader.h"
#include <stdexcept>
#include <iostream>

// Using stb_image - a popular single-header image loading library
// Download from: https://github.com/nothings/stb
#include "stb_image.h"

namespace sgraph
{
    /**
     * @brief This class is used to load an image in the PNG format using stb_image
     * 
     * Features:
     * - Supports PNG, JPEG, BMP, TGA, and other formats via stb_image
     * - Automatically handles different channel counts (grayscale, RGB, RGBA)
     * - Flips image vertically to match OpenGL texture coordinate system
     * - Converts all images to RGB format for consistency with PPM loader
     */
    class PNGImageLoader: public ImageLoader {
   
        public:
            PNGImageLoader() {
                // Enable vertical flip to match OpenGL's bottom-left origin
                stbi_set_flip_vertically_on_load(true);
            }
   
            void load(string filename) {
                int channels;
                
                // Load the image using stb_image
                // Force 3 channels (RGB) to match the PPM loader behavior
                GLubyte* loaded_image = stbi_load(filename.c_str(), &width, &height, &channels, 3);
                
                if (!loaded_image) {
                    throw std::invalid_argument("Failed to load image: " + string(stbi_failure_reason()));
                }
                
                std::cout << "Image file loaded: " << filename << std::endl;
                std::cout << "Dimensions: " << width << "x" << height << std::endl;
                std::cout << "Original channels: " << channels << " (converted to 3)" << std::endl;
                
                // Allocate memory for our image array
                image = new GLubyte[3 * width * height];
                
                // Copy the loaded image data
                // stb_image already handles the vertical flip for us
                for (int i = 0; i < 3 * width * height; i++) {
                    image[i] = loaded_image[i];
                }
                
                // Free the stb_image allocated memory
                stbi_image_free(loaded_image);
            }
            
            /**
             * @brief Alternative load method that preserves original channel count
             * Use this if you need to handle alpha channels or grayscale images
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
                std::cout << "Channels: " << channels << std::endl;
                
                // Allocate memory based on actual channel count
                image = new GLubyte[channels * width * height];
                
                // Copy the loaded image data
                for (int i = 0; i < channels * width * height; i++) {
                    image[i] = loaded_image[i];
                }
                
                // Store channel count for later use
                this->channels = channels;
                
                // Free the stb_image allocated memory
                stbi_image_free(loaded_image);
            }
            
        private:
            int channels = 3; // Default to RGB
    };
}

#endif