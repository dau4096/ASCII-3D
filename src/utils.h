/* utils.h */

#ifndef UTILS_H
#define UTILS_H


#include "global.h"
#include "constants.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include <algorithm>

using namespace std;



//Utility functions
namespace utils {
	static inline void hideConsole() {
	#ifdef _WIN32
		ShowWindow(GetConsoleWindow(), SW_HIDE);
	#endif
	}

	static inline void showConsole() {
	#ifdef _WIN32
		ShowWindow(GetConsoleWindow(), SW_SHOW);
	#endif
	}

	static inline bool isConsoleVisible() {
	#ifdef _WIN32
		return IsWindowVisible(GetConsoleWindow());
	#elif __linux__
		return true;
	#endif 
	}



	static inline void raise(std::string err) {
		std::cerr << err << std::endl;
		std::string end;
		std::cin >> end;
	}
	static inline void pause() {
		string pause;
		std::cin >> pause;
	}




	static inline std::string strToLower(const std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){return std::tolower(c);});
		return result;
	}

	static inline std::string strToUpper(const std::string& input) {
		std::string result = input;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c){return std::toupper(c);});
		return result;
	}



	template<typename T>
	static inline void combineVectors(std::vector<T>* A, std::vector<T>& B) {
		A->insert(A->end(), B.begin(), B.end());
	}


	int RNGc(); //Client
	int RNGw(); //World
	void clearRNG(); //Reset both
}

#endif