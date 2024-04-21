// Info
// Before using for the first time, please read the readme file

#include <vector>
#include <string>
#include <functional>

#include "opencv2/opencv.hpp"

// Place below the definitions of your custom functions for stages, as well as their aliases

// set alias for user function (aka stage callable)
const std::string nameCallable{ "dillation" };

// example user function for stage
// replace the function body with the one you need
cv::Mat stageCallable(cv::Mat arg) {

	static cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

	cv::Mat res;
	cv::dilate(arg, res, kernel);

	return res;
}

// Below you need to add the parameters created above to the vector. It is enough to correct the example code below (line that starts with "choosers->push_back")

#ifdef __linux__



#elif _WIN32

#include <windows.h>

extern "C" {

	__declspec(dllexport) void* __cdecl getChooser(void* arg) {

		std::vector<std::pair<std::string, std::function<cv::Mat(cv::Mat)>>>* choosers = reinterpret_cast<std::vector<std::pair<std::string, std::function<cv::Mat(cv::Mat)>>>*>(arg);

		choosers->push_back({ nameCallable, std::function<cv::Mat(cv::Mat)>{stageCallable} });

		return arg;
	}
}

#else

#error Platform not supported!

#endif