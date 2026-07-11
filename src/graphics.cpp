/* graphics.cpp */

#include <stb_image.h>
#include <stb_image_write.h>
#include <math.h>
#include <format>
#include <regex>
#include <string>


//Include GLEW.
#include <GL/glew.h>


//Include OpenGL.
#include <GL/gl.h>
#include <GL/glu.h>


//Include GLM.
//#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtc/type_ptr.hpp>


//Include GLFW
#include <GLFW/glfw3.h>



#include "global.h"
#include "utils.h"




void APIENTRY openGLErrorCallback(
		GLenum source,
		GLenum type, GLuint id,
		GLenum severity,
		GLsizei length, const GLchar* message,
		const void* userParam
	) {
	/*
	Nicely formatted callback from;
	[https://learnopengl.com/In-Practice/Debugging]
	*/
	if ((id == 131169u) || (id == 131185u) || (id == 131218u) || (id == 131204u)) {return; /* Ignored warning IDs that are not errors */}

	std::cout << "---------------" << std::endl << "Debug message (" << id << ") | " << message << std::endl;

	switch (source)
	{
		case GL_DEBUG_SOURCE_API:             {std::cout << "Source: API"; break;}
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   {std::cout << "Source: Window System"; break;}
		case GL_DEBUG_SOURCE_SHADER_COMPILER: {std::cout << "Source: Shader Compiler"; break;}
		case GL_DEBUG_SOURCE_THIRD_PARTY:     {std::cout << "Source: Third Party"; break;}
		case GL_DEBUG_SOURCE_APPLICATION:     {std::cout << "Source: Application"; break;}
		case GL_DEBUG_SOURCE_OTHER:           {std::cout << "Source: Other"; break;}
	} std::cout << std::endl;

	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR:               {std::cout << "Type: Error"; break;}
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: {std::cout << "Type: Deprecated Behaviour"; break;}
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  {std::cout << "Type: Undefined Behaviour"; break;} 
		case GL_DEBUG_TYPE_PORTABILITY:         {std::cout << "Type: Portability"; break;}
		case GL_DEBUG_TYPE_PERFORMANCE:         {std::cout << "Type: Performance"; break;}
		case GL_DEBUG_TYPE_MARKER:              {std::cout << "Type: Marker"; break;}
		case GL_DEBUG_TYPE_PUSH_GROUP:          {std::cout << "Type: Push Group"; break;}
		case GL_DEBUG_TYPE_POP_GROUP:           {std::cout << "Type: Pop Group"; break;}
		case GL_DEBUG_TYPE_OTHER:               {std::cout << "Type: Other"; break;}
	} std::cout << std::endl;
	
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_HIGH:         {std::cout << "Severity: high"; break;}
		case GL_DEBUG_SEVERITY_MEDIUM:       {std::cout << "Severity: medium"; break;}
		case GL_DEBUG_SEVERITY_LOW:          {std::cout << "Severity: low"; break;}
		case GL_DEBUG_SEVERITY_NOTIFICATION: {std::cout << "Severity: notification"; break;}
	} std::cout << std::endl << std::endl;

#ifdef PAUSE_ON_OPENGL_ERROR
	utils::pause();
#endif
}








namespace graphics {




GLFWwindow* initialiseWindow(glm::ivec2 resolution, const char* title) {
	if (!glfwInit()) {
		utils::raise("Failed to initialize GLFW");
		return nullptr;
	}


	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, constants::display::OPENGL_VERSION_MAJOR);  //OpenGL major ver (4)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, constants::display::OPENGL_VERSION_MINOR);  //OpenGL minor ver (6)
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  //Use Core (not ES)


	GLFWwindow* Window = glfwCreateWindow(resolution.x, resolution.y, title, NULL, NULL);
	if (!Window) {
		glfwTerminate();
		utils::raise("Failed to create GLFW window");
		return nullptr;
	}
	glfwMakeContextCurrent(Window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		utils::raise("Failed to initialize GLEW.");
	}


	//glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return Window;
}









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






namespace uniforms {

//Uniforms; [Many overloads]
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, bool value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, unsigned int value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1ui(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, int value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1i(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, float value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform1f(location, value);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec2 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2i(location, value.x, value.y);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::uvec2 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2ui(location, value.x, value.y);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec2 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform2f(location, value.x, value.y);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec3 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3i(location, value.x, value.y, value.z);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::uvec3 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3ui(location, value.x, value.y, value.z);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec3 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform3f(location, value.x, value.y, value.z);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::ivec4 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4i(location, value.x, value.y, value.z, value.w);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::uvec4 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4ui(location, value.x, value.y, value.z, value.w);
	}
}
static inline void bindUniformValue(GLuint shaderProgram, const GLchar* uniformName, glm::vec4 value) {
	GLint location = glGetUniformLocation(shaderProgram, uniformName);
	if (location >= 0) {
		glUniform4f(location, value.x, value.y, value.z, value.w);
	}
}

}





static unsigned int lineNumberAt(const std::string& s, size_t pos) {
	//Find [#line] number from position
	return std::count(s.begin(), s.begin() + pos, '\n');
}

std::string preprocessIncludes(const std::string& source, const std::string& currentFile) {
	std::regex includeRegex(R"(^\s*#include\s*<([^>]+)>)", std::regex_constants::multiline);

	std::string result;
	std::sregex_iterator it(source.begin(), source.end(), includeRegex);
	std::sregex_iterator end;

	size_t lastPos = 0;
	for (; it!=end; it++) {
		const std::smatch& match = *it;

		//Copy text before include
		result.append(source.substr(lastPos, match.position() - lastPos));

		std::string includeFile = match[1].str();
		std::string includePath = "src/shaders/" + includeFile + ".glsl";

		std::string includedSource = utils::readFile(includePath);

		unsigned int includeLine = lineNumberAt(source, match.position());

	#ifdef LINE_DIRECTIVE_STRING
		//Can be format `#line [lnNum] [srcFile]`
		result += "#line 1 \"src/shaders/"+includeFile+".glsl\"\n"+includedSource+"\n"+"#line "+std::to_string(includeLine+1u)+" \""+currentFile+"\"\n";
	#else
		//Must be of format `#line [lnNum]`
		result += "#line 1 \n"+includedSource+"\n"+"#line "+std::to_string(includeLine+1u)+" \n";
	#endif
		
		lastPos = match.position() + match.length();
	}

	// Append remaining source
	result.append(source.substr(lastPos));

	return result;
}



GLuint compileShader(GLenum shaderType, string filePath) {
	std::string source = utils::readFile(filePath);
	source = preprocessIncludes(source, filePath);
	const char* src = source.c_str();

	//Create a shader id
	GLuint shader = glCreateShader(shaderType);
	if (shader == 0) {
		utils::raise("Error: Failed to create shader.");
		return 0;
	}

	//Attach the shader src
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);
	

	//Errorcheck
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infolog);
		utils::raise("Error: Shader compilation failed;\n" + string(infolog));
	}

	return shader;
}





GLuint createComputeShader(std::string compShaderName) {
	GLuint computeShader = compileShader(GL_COMPUTE_SHADER, "src/shaders/" + compShaderName);

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, computeShader);
	glLinkProgram(shaderProgram);

	GLint success;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		if (!utils::isConsoleVisible()) {
			utils::showConsole();
		}
		char infolog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infolog);
		utils::raise("Error: Compute shader program linking failed:\n" + std::string(infolog));
	}

	glDeleteShader(computeShader);

	return shaderProgram;
}




GLuint imageToOpenGL(const unsigned char* pxData, const glm::uvec2 resolution) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, resolution.x, resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, pxData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


GLuint createImage(const GLuint format, const glm::uvec2 resolution) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexStorage2D(GL_TEXTURE_2D, 1, format, resolution.x, resolution.y);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


void getASCIIdataFromTexture(const GLuint imageID, unsigned char* asciiData, const glm::uvec2 resolution) {
	glMemoryBarrier(GL_TEXTURE_UPDATE_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glGetTextureImage(
		imageID, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE,
		resolution.x * resolution.y * sizeof(unsigned char),
		asciiData
	);
}



GLuint createShaderStorageBufferObject(int binding, size_t bufferSize=0, GLuint glType=GL_DYNAMIC_DRAW) {
	GLuint SSBO;
	glGenBuffers(1, &SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, glType);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, SSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	return SSBO;
}


template<typename T>
void updateShaderStorageBufferObject(
	GLuint SSBO,
	T* data,
	size_t count
) {
	size_t size = sizeof(T) * count;

	if (count > 0) {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, data);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}



void createCharsSSBO(unsigned int binding, const std::string chars) {
	std::vector<unsigned int> charsData;
	for (const char& c : chars) {charsData.push_back((unsigned int)(c));}
	GLuint charsSSBO = createShaderStorageBufferObject((int)(binding), chars.size() * sizeof(unsigned int));
	updateShaderStorageBufferObject(charsSSBO, charsData.data(), chars.size());
	numAsciiChars = chars.size();
	utils::GLErrorcheck("charsSSBO", true);
}


#define KERNEL_WIDTH 13
#define KERNEL_RADIUS (KERNEL_WIDTH / 2) /* Integer divide */
#define KERNEL_COUNT (KERNEL_WIDTH * KERNEL_WIDTH)
void createGaussianSSBO(unsigned int binding, const float sigma) {
	float kernel[KERNEL_COUNT];
	for (int y=-KERNEL_RADIUS; y<=KERNEL_RADIUS; y++) {
		for (int x=-KERNEL_RADIUS; x<=KERNEL_RADIUS; x++) {
			kernel[(y+KERNEL_RADIUS) * KERNEL_WIDTH + x+KERNEL_RADIUS] = std::exp(-(float)(x*x + y*y)/(2.0f*sigma*sigma));
		}
	}

	GLuint gaussSSBO = createShaderStorageBufferObject((int)(binding), KERNEL_COUNT * sizeof(float)); //Always a 13*13 kernel.
	updateShaderStorageBufferObject(gaussSSBO, kernel, KERNEL_COUNT);
	utils::GLErrorcheck("gaussSSBO_" + std::to_string(binding-1), true);
}



void initialise(void) {
	//Set up OpenGL and other required data.


	//Buffer to store character sequence data
	const std::string asciiChars = " `.':_,^=;><+!rc*z?sLTv)J7(Fi{C}fI31tlu[neoZ5Yxjya]2ESwqkP6h9d4VpOGbUAKXHm8RD#$Bg0MNWQ%&@"; //[https://stackoverflow.com/a/74186686]
	//const std::string asciiChars = ".'`^\",:;Il!i><~+_?][}{1)(tfjrxnuvczXYUJCLQ0OZmwqpdbkhao*#MW&8%B@$"; //[https://stackoverflow.com/a/67780964]
	//const std::string asciiChars = " .;coPO?@#"; //Acerola : [https://youtu.be/gg40RWiaHRY]
	createCharsSSBO(0u, asciiChars); //Binding=0


	//Buffer to store Blur-1's gaussian kernel (sigma = 1.0f)
	createGaussianSSBO(1u, 1.0f); //Binding=1

	//Buffer to store Blur-2's gaussian kernel (sigma = 2.0f)
	createGaussianSSBO(2u, 2.0f); //Binding=2


	//Load shaders
	GLIndex::luminanceShader = createComputeShader("ascii/luminance.comp");
	GLIndex::gaussianShader = createComputeShader("ascii/gaussian.comp");
	GLIndex::edgeShader = createComputeShader("ascii/edges.comp"); //Contains Difference of Gaussians and Sobel filter



	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLErrorCallback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
}



//graphics::ascii (GPU)
namespace ascii {

	void luminanceCompute(const GLuint inputImageID, const GLuint asciiImageID, const GLuint dataImageID, const glm::uvec2 resolution);
	void edgeCompute(const GLuint dataImageID, const GLuint asciiImageID, const glm::uvec2 resolution);

	void processImage(const unsigned char* pxData, unsigned char* asciiData, const glm::uvec2 resolution) {
		//Convert image to opengl format
		GLuint inputImageID = imageToOpenGL(pxData, resolution);
		//Create image to store ascii data
		GLuint asciiImageID = createImage(GL_R8UI, resolution);
		//Image to store luminance values (original image: R, blur 1: G, blur 2: B, is-Edge: A), 
		GLuint dataImageID = createImage(GL_RGBA8UI, resolution);


		//Run luminance pass
		luminanceCompute(inputImageID, asciiImageID, dataImageID, resolution);


		//Run edge passes
		edgeCompute(dataImageID, asciiImageID, resolution);


		//Get ascii data from opengl image
		getASCIIdataFromTexture(asciiImageID, asciiData, resolution);
	}


	#define DISPATCH_SIZE_2D glm::uvec3(16u, 16u, 1u)	
	#define COMPUTE_AXIS(AX, VAR) (((VAR) + (AX) - 1u) / (AX))
	void luminanceCompute(
		const GLuint inputImageID, const GLuint asciiImageID,
		const GLuint dataImageID, const glm::uvec2 resolution
	) {
		//Runs luminance.comp
		glUseProgram(GLIndex::luminanceShader);
		uniforms::bindUniformValue(GLIndex::luminanceShader, "numAsciiChars", (unsigned int)(numAsciiChars));
		glBindTextureUnit(0, inputImageID);
		glBindImageTexture(1, asciiImageID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
		glBindImageTexture(2, dataImageID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI);
		glDispatchCompute(
			COMPUTE_AXIS(DISPATCH_SIZE_2D.x, resolution.x),
			COMPUTE_AXIS(DISPATCH_SIZE_2D.y, resolution.y),
			1u
		);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}


	void edgeCompute(
		const GLuint dataImageID, const GLuint asciiImageID,
		const glm::uvec2 resolution
	) {
		//Runs gaussian.comp and edge.comp
		//Runs both blurs, in series (Avoids race condition)
		glUseProgram(GLIndex::gaussianShader);
		uniforms::bindUniformValue(GLIndex::gaussianShader, "resolution", resolution);
		uniforms::bindUniformValue(GLIndex::gaussianShader, "gaussianRadius", (unsigned int)(KERNEL_RADIUS));
		glBindImageTexture(2, dataImageID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
		GLint selKernelLoc = glGetUniformLocation(GLIndex::gaussianShader, "selGaussKernel");
		for (unsigned int i=0u; i<2u; i++) {
			glUniform1ui(selKernelLoc, (bool)(i));
			glDispatchCompute(
				COMPUTE_AXIS(DISPATCH_SIZE_2D.x, resolution.x),
				COMPUTE_AXIS(DISPATCH_SIZE_2D.y, resolution.y),
				1u
			);
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}



		glUseProgram(GLIndex::edgeShader);
		uniforms::bindUniformValue(GLIndex::gaussianShader, "resolution", resolution);
		glBindImageTexture(1, asciiImageID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R8UI);
		glBindImageTexture(2, dataImageID, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI);
		glDispatchCompute(
			COMPUTE_AXIS(DISPATCH_SIZE_2D.x, resolution.x),
			COMPUTE_AXIS(DISPATCH_SIZE_2D.y, resolution.y),
			1u
		);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	}

}


}




//ascii (CPU)
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


#ifdef USE_PERCEPTUAL_LUMINANCE
	return (
		0.2126f * sRGBtoLin(vR) +
		0.7152f * sRGBtoLin(vG) +
		0.0722f * sRGBtoLin(vB)
	);

#else
	//More uniform luminance, but not as "visually accurate".
	return (vR + vG + vB) / 3.0f;
#endif
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

			if ((R == 0) && (G == 0) && (B == 0)) {std::cout << ' '; continue;}


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