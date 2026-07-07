/* graphics.cpp */

#include "global.h"
#include "utils.h"
#include <stb_image.h>
#include <stb_image_write.h>



namespace graphics {


int loadImage(const std::string path, unsigned char** pxData, size_t* width=nullptr, size_t* height=nullptr) {
	int w, h, c;
	unsigned char* textureDataSTBI = stbi_load(
		path.c_str(), &w, &h, &c, 3 //Only take RGB
	);

	if (!textureDataSTBI) {return FALSE; /* Failed to load. */}

	if (width) {*width = w;}
	if (height) {*height = h;}

	*pxData = textureDataSTBI;
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



const std::string asciiChars = " `.-':_,^=;><+!rc*/z?sLTv)J7(|Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"; //[https://stackoverflow.com/a/74186686]
//const std::string asciiChars = ".'`^\",:;Il!i><~+_-?][}{1)(|\\/tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"; //[https://stackoverflow.com/a/67780964]
//const std::string asciiChars = " .;coPO?@#"; //Acerola : [https://youtu.be/gg40RWiaHRY]
void luminance(const unsigned char* pxData, unsigned char* asciiData, float* luminanceData, const size_t width, const size_t height) {

	for (unsigned int y=0u; y<height; y++) {
		for (unsigned int x=0u; x<width; x++) {
			unsigned int index = (y * width) + x;
			float luminanceFloat = getLuminanceOfRGB(
				*(pxData + (index * 3u)), //R
				*(pxData + (index * 3u) + 1u), //G
				*(pxData + (index * 3u) + 2u) //B
			);
			luminanceData[index] = luminanceFloat;

			int luminanceInt = (int)(luminanceFloat * (float)(asciiChars.length()));
			*(asciiData + index) = asciiChars[luminanceInt];
		}
	}
}



void gaussian(const float* luminanceData, float* gaussianData, const size_t width, const size_t height, const float sigma) {
	//Gaussian blur with given strength.
	float kernel[13][13];
	for (int y=-6; y<=6; y++) {
	    for (int x=-6; x<=6; x++) {
	        kernel[y+6][x+6] = std::exp(-(x*x + y*y)/(2.0f*sigma*sigma));
	    }
	}


    for (size_t y=0u; y<height; y++) {
        for (size_t x=0u; x<width; x++) {
            float sum = 0.0f;
		    float kernelSum = 0.0f;

            for (int ky=-6; ky<=6; ky++) {
                for (int kx=-6; kx<=6; kx++) {
                    size_t ix = x + kx;
                    size_t iy = y + ky;

                    if (((int)(x) + kx < 0) || ((int)(y) + ky < 0) || ((int)(x) + kx >= width) || ((int)(y) + ky >= height)) {continue;}

                    float kVal = kernel[ky+6][kx+6];
                    sum += luminanceData[iy * width + ix] * kVal;
			        kernelSum += kVal;
                }
            }

            gaussianData[y * width + x] = std::clamp(sum / kernelSum, 0.0f, 1.0f);
        }
    }
}


void differenceOfGaussians(const float* luminanceData, unsigned char* DoGdata, const size_t width, const size_t height) {
	//Delta of 2 gaussian blurs of differing strengths.
    std::vector<float> blur1 = std::vector<float>(width * height);
    std::vector<float> blur2 = std::vector<float>(width * height);

    gaussian(luminanceData, blur1.data(), width, height, 1.0f);
    gaussian(luminanceData, blur2.data(), width, height, 2.0f);


    for (size_t i=0u; i<width*height; i++) {
        float diff = blur2[i] - blur1[i];

        DoGdata[i] = static_cast<unsigned char>((diff + 1.0f) * 127.5f);
    }
}




void edges(const float* luminanceData, unsigned char* asciiData, const size_t width, const size_t height) {
	//Difference of Gaussians to find areas of high luminance change
	std::vector<unsigned char> DoGauss = std::vector<unsigned char>(width * height);
	differenceOfGaussians(luminanceData, DoGauss.data(), width, height);

	//Sobel filter for edge direction

	//Draw ascii characters representing edges from sobel data


	/*
	//DEBUG
    for (size_t i=0u; i<width*height; i++) {
    	asciiData[i] = (DoGauss[i] > 127) ? '#' : ' ';
    }*/
}








void draw(const unsigned char* asciiData, const size_t width, const size_t height) {
	for (size_t y=0u; y<height; y++) {
		std::string line = std::string(
			reinterpret_cast<const char*>(asciiData + y * width), width
		);
		std::cout << line << '\n';
	}
}


}