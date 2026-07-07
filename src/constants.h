/* constants.h */

#pragma once

#include <numbers>
#include <limits>


#define TRUE 1
#define FALSE 0


namespace constants {
	//Mathematical Constants
	constexpr float PI = std::numbers::pi_v<float>;
	constexpr float PI2 = PI * 2.0f;
	constexpr float EXP = std::numbers::e_v<float>;
	constexpr float INF = std::numeric_limits<float>::infinity();

	constexpr float TO_RAD = 0.017453f;
	constexpr float TO_DEG = 57.29577f;

}