/* graphics.h */
#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "global.h"
#include "utils.h"




namespace graphics {

	int loadImage(const std::string path, unsigned char** pxData, size_t* width=nullptr, size_t* height=nullptr);

}



namespace ascii {

	void luminance(unsigned char* pxData, unsigned char* asciiData, const size_t width, const size_t height);
	void edges(unsigned char* pxData, unsigned char* asciiData, const size_t width, const size_t height);

	void draw(unsigned char* asciiData, const size_t width, const size_t height);

}


#endif


