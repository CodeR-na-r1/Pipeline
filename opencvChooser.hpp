#pragma once

#include <string>
#include <filesystem>
#include <functional>

#include "opencv2/opencv.hpp"

struct OpencvChooser {

	cv::Mat logo;

	OpencvChooser(cv::Mat logo) :logo(logo) {}

	std::function<cv::Mat(cv::Mat)> operator()(const std::string callableName) {

		if (callableName == "display") {

			return [](cv::Mat data) {
				cv::imshow("Display...", data);
				cv::waitKey(1);
				return data;
				};
		}
		else if (callableName == "rgb2gray") {

			return [](cv::Mat data) { 
				cv::Mat res;
				cv::cvtColor(data, res, cv::COLOR_BGR2GRAY);
				return res;
				};
		}
		else if (callableName == "gaussian") {

			return [](cv::Mat data) { 
				cv::Mat res;
				cv::GaussianBlur(data, res, cv::Size(3, 3), 0);
				return res;
				};
		}
		else if (callableName == "circle") {

			return [](cv::Mat data) { 
				cv::circle(data, cv::Point(300, 300), 3, cv::Scalar(1, 1, 1));
				return data;
				};
		}
		else if (callableName == "drawLogo") {

			return [logo=this->logo](cv::Mat data) { 
				int x = data.cols - logo.cols;	// code from here: https://datahacker.rs/opencv-pixel-intensity-change-and-watermark/#id4
				int y = data.rows - logo.rows;
				cv::Mat imageROI = data(cv::Rect(x, y, logo.cols, logo.rows));
				cv::Mat invSrc = cv::Scalar::all(255) - logo;
				cv::Mat mask(invSrc);
				invSrc.copyTo(imageROI, mask);
				return data; };
		}
		else if (callableName == "store1") {

			return [](cv::Mat data) {
				static uint64_t counter{};
				cv::imwrite(std::filesystem::path{ "../storeRes/store1/frame_" + std::to_string(++counter) + ".jpg" }.generic_string(), data);
				return data; };
		}
		else if (callableName == "store2") {

			return [](cv::Mat data) {
				static uint64_t counter{};
				cv::imwrite(std::filesystem::path{ "../storeRes/store2/frame_" + std::to_string(++counter) + ".jpg" }.generic_string(), data);
				return data; };
		}
		else if (callableName == "coords") {

			return [](cv::Mat data) { return data; };
		}
		
		return [](cv::Mat data) { return data; };
	}
};

// Used as a plug
// Designed to simplify the development and debugging process
struct OpencvLightChooser {

	OpencvLightChooser() {}
	OpencvLightChooser(cv::Mat logo) {}

	std::function<cv::Mat(cv::Mat)> operator()(const std::string callableName) {

		if (callableName == "display") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "rgb2gray") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "gaussian") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "circle") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "drawLogo") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "store1") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "store2") {

			return [](cv::Mat data) { return data; };
		}
		else if (callableName == "coords") {

			return [](cv::Mat data) { return data; };
		}

		return [](cv::Mat data) { return data; };
	}
};