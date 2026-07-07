/* graphics.cpp */

#include "global.h"
#include "utils.h"
#include <stb_image.h>
#include <stb_image_write.h>
#include <math.h>
#include <format>



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

	return (vR + vG + vB) / 3.0f;

	/*
	return (
		0.2126f * sRGBtoLin(vR) +
		0.7152f * sRGBtoLin(vG) +
		0.0722f * sRGBtoLin(vB)
	);
	*/
}
//https://stackoverflow.com/a/56678483
//// sRGB LUMINANCE ////



const std::string asciiChars = " `.':_,^=;><+!rc*z?sLTv)J7(Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"; //[https://stackoverflow.com/a/74186686]
//const std::string asciiChars = ".'`^\",:;Il!i><~+_?][}{1)(tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"; //[https://stackoverflow.com/a/67780964]
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


void differenceOfGaussians(const float* luminanceData, float* DoGdata, const size_t width, const size_t height) {
	//Delta of 2 gaussian blurs of differing strengths.
	std::vector<float> blur1 = std::vector<float>(width * height);
	std::vector<float> blur2 = std::vector<float>(width * height);

	gaussian(luminanceData, blur1.data(), width, height, 1.0f);
	gaussian(luminanceData, blur2.data(), width, height, 2.0f);


	for (size_t i=0u; i<width*height; i++) {
		DoGdata[i] = blur2[i] - blur1[i];
	}
}




void sobel(const float* inputData, float* sobelAngles, const size_t width, const size_t height) {
	//Use sobel filter on difference of gaussians data, then [atan2() + Pi] to get angle. If no edge, angle is some negative value (ignored later)
	static constexpr int Gx[3][3] = {
		{-1, 0, 1},
		{-2, 0, 2},
		{-1, 0, 1}
	};
	static constexpr int Gy[3][3] = {
		{-1, -2, -1},
		{ 0,  0,  0},
		{ 1,  2,  1}
	};
	constexpr float edgeThreshold = 0.25f;



	//Mark everything as "not an edge"
	std::fill(sobelAngles, sobelAngles + width * height, -1.0f);

	for (size_t y=1; y<height-1; y++) {
		for (size_t x=1; x<width-1; x++) {
			float gx = 0.0f, gy = 0.0f;

			for (int ky=-1; ky<=1; ky++) {
				for (int kx=-1; kx<=1; kx++) {
					float p = inputData[(y + ky) * width + (x + kx)];

					gx += p * Gx[ky + 1][kx + 1];
					gy += p * Gy[ky + 1][kx + 1];
				}
			}


			float mag = std::sqrt(gx * gx + gy * gy);
			if (mag < edgeThreshold) {continue;}

			float angle = std::atan2(gy, gx);
			if (angle < 0.0f) {angle += constants::PI2;}

			float edgeAngle = angle + constants::PI / 2.0f;
			if (edgeAngle >= 2.0f * std::numbers::pi_v<float>) {edgeAngle -= constants::PI2;}

			sobelAngles[y * width + x] = edgeAngle;
		}
	}
}



char edgeChar(float angle) {
	angle += constants::PI / 8.0f; //Centre the bins
	if (angle >= 2*constants::PI) {angle -= constants::PI2;}

	int sector = (int)(angle / (constants::PI/4.0f)) & 0b111;
	switch (sector) {
		case 0b000: case 0b100: {
			return '|';
		}

		case 0b001: case 0b101: {
			return '/';
		}

		case 0b010: case 0b110: {
			return '-';
		}

		case 0b011: case 0b111: {
			return '\\';
		}

		default: {
			return '?';
		}
	}
}



void edges(const float* luminanceData, unsigned char* asciiData, const size_t width, const size_t height) {
	//Difference of Gaussians to find areas of high luminance change
	std::vector<float> DoGauss = std::vector<float>(width * height);
	differenceOfGaussians(luminanceData, DoGauss.data(), width, height);

	//Sobel filter for edge direction
	std::vector<float> sobelAngles = std::vector<float>(width * height); //Angle of each px. Negative if not an edge. (If edge, will be in range 0-2pi)
	sobel(DoGauss.data(), sobelAngles.data(), width, height);

	//Draw ascii characters representing edges from sobel data
	for (unsigned int i=0u; i<width*height; i++) {
		float edgeAngle = sobelAngles[i];
		if (edgeAngle >= 0.0f) {
			//Replace with edge character
			asciiData[i] = edgeChar(edgeAngle + constants::PI/2.0f);
		}
	}
}








void draw(const unsigned char* pxData, const unsigned char* asciiData, const size_t width, const size_t height) {
	for (size_t y = 0; y < height; ++y) {
		unsigned char lastR = 255u, lastG = 255u, lastB = 255u;
		bool first = true;
		for (size_t x = 0; x < width; ++x) {
			size_t index = (y * width + x) * 3u;

			unsigned char R = pxData[index + 0u];
			unsigned char G = pxData[index + 1u];
			unsigned char B = pxData[index + 2u];

			if (
				first ||
				(R != lastR) ||
				(G != lastG) ||
				(B != lastB)
			) {
				std::cout << "\x1b[38;2;" << int(R) << ';' << int(G) << ';' << int(B) << 'm';

				lastR = R; lastG = G; lastB = B;
				first = false;
			}

			std::cout << asciiData[index / 3u];
		}

		std::cout << "\x1b[0m\n";
	}
}


}