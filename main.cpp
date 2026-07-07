/* main.cpp */

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include "src/global.h"
#include "src/utils.h"
#include "src/graphics.h"







int main() {
	try { //Catch exceptions

#ifdef __WIN32
	SetConsoleOutputCP(65001); //CP_UTF8, Windows.
#else
	#pragma execution_character_set("utf-8") //Linux.
#endif


	size_t width, height; //Assumes same number of ASCII characters as pixels.
	unsigned char* imageData;
	//Will eventually be replaced with OpenGL scene rendered at low res.
	if (!graphics::loadImage("images/a.png", &imageData, &width, &height)) {
		//Failed
		std::cerr << "Failed to load image." << std::endl;
		return -1;
	}
	std::cout << "Loaded image with res: (" << width << ", " << height << ")" << std::endl;

	unsigned char* asciiData = (unsigned char*)calloc(width * height, sizeof(unsigned char));
	float* luminanceData = (float*)calloc(width * height, sizeof(float));
	ascii::luminance(imageData, asciiData, luminanceData, width, height);
	ascii::edges(luminanceData, asciiData, width, height); //TBA
	ascii::draw(asciiData, width, height);
	stbi_image_free(imageData);



	//Catch exceptions.
	} catch (const std::exception& e) {
		if (!utils::isConsoleVisible()) {utils::showConsole();}
		std::cerr << "An exception was thrown: " << e.what() << std::endl;
		utils::pause();
		return -1;
	} catch (...) {
		if (!utils::isConsoleVisible()) {utils::showConsole();}
		std::cerr << "An unspecified exception was thrown." << std::endl;
		utils::pause();
		return -1;
	}
}
