/* constants.h */

#pragma once

#include <glm/glm.hpp>


#define TRUE 1
#define FALSE 0


namespace constants {
	//Mathematical Constants
	constexpr float PI = 3.141592f;
	constexpr float PI2 = PI * 2.0f;
	constexpr float EXP = 2.718281f;
	constexpr float INF = std::numeric_limits<float>::infinity();

	constexpr float TO_RAD = 0.017453f;
	constexpr float TO_DEG = 57.29577f;

}