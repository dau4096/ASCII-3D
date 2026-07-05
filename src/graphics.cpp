/* graphics.cpp */

#include "global.h"
#include "utils.h"
#include <stb_image.h>
#include <stb_image_write.h>



namespace graphics {


int loadImage(const std::string path, unsigned char** pxData, size_t* width=nullptr, size_t* height=nullptr) {
	int w, h, c;
	unsigned char* textureDataSTBI = stbi_load(
		path.c_str(),
		&w, &h,
		&c, 3 //Only take RGB back, not A.
	);

	if (!textureDataSTBI) {return FALSE; /* Failed to load. */}

	if (width) {*width = w;}
	if (height) {*height = h;}

	*pxData = textureDataSTBI;
	stbi_image_free(textureDataSTBI);
	return TRUE;
}


}



namespace ascii {

//// sRGB LUMINANCE ////
//https://stackoverflow.com/a/56678483
float sRGBtoLin(float colourChannel) {
	//Send this function a decimal sRGB gamma encoded color value
	//between 0.0 and 1.0, and it returns a linearized value.

	if (colourChannel <= 0.04045f) {
		return colourChannel / 12.92f;
	} else {
		return std::pow(
			(colourChannel + 0.055f) / 1.055f,
			2.4f
		);
	}
}


float getLuminanceOfRGB(unsigned char R, unsigned char G, unsigned char B) {
	float vR = (float)(R) / 255.0f;
	float vG = (float)(G) / 255.0f;
	float vB = (float)(B) / 255.0f;

	return (
		0.2126f * sRGBtoLin(vR) +
		0.7152f * sRGBtoLin(vG) +
		0.0722f * sRGBtoLin(vB)
	);
}
//https://stackoverflow.com/a/56678483
//// sRGB LUMINANCE ////



void luminance(unsigned char* pxData, unsigned char* asciiData, const size_t width, const size_t height) {
	const std::string asciiChars = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@";
	for (unsigned int y=0u; y<height; y++) {
		for (unsigned int x=0u; x<width; x++) {
			unsigned int index = (y * width) + x;
			float luminanceFloat = getLuminanceOfRGB(
				*(pxData + (index * 3u)), //R
				*(pxData + (index * 3u) + 1u), //G
				*(pxData + (index * 3u) + 2u) //B
			);

			int luminanceInt = (int)(luminanceFloat * (float)(asciiChars.length()));
			*(asciiData + index) = asciiChars[luminanceInt];
		}
	}
}


void edges(unsigned char* pxData, unsigned char* asciiData, const size_t width, const size_t height) {

}





void draw(unsigned char* asciiData, const size_t width, const size_t height) {
	for (size_t y=0u; y<height; y++) {
		std::string line = std::string(
			reinterpret_cast<const char*>(asciiData + y * width), width
		);

		std::cout << line << '\n';
	}
}


}