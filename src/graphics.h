/* graphics.h */
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <glm/glm.hpp>

#include "global.h"
#include "utils.h"




namespace graphics {

	GLFWwindow* initialiseWindow(glm::ivec2 resolution, const char* title);

	int loadImage(const std::string path, unsigned char** pxData, size_t* width=nullptr, size_t* height=nullptr);

	void initialise(void);

	namespace ascii {

		void processImage(const unsigned char* pxData, unsigned char* asciiData, const glm::uvec2 resolution);

	}


}



namespace ascii {

	void luminance(const unsigned char* pxData, unsigned char* asciiData, float* luminanceData, const size_t width, const size_t height);
	void edges(const float* luminanceData, unsigned char* asciiData, const size_t width, const size_t height);


	void draw(const unsigned char* pxData, const unsigned char* asciiData, const size_t width, const size_t height);

}


#endif


