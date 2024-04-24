#pragma once

#include <string>
#include <vector>
#include <filesystem>
#include <functional>

#include "opencv2/opencv.hpp"

struct OpencvChooser {

	cv::Mat logo;

	OpencvChooser(cv::Mat logo) :logo(logo) {}

	const std::vector<std::pair<std::string, std::function<cv::Mat(cv::Mat)>>> callables{
		{ "display", [](cv::Mat data) { cv::imshow("Display...", data); cv::waitKey(1); return data; } },
		{ "rgb2gray", [](cv::Mat data) { cv::Mat res; cv::cvtColor(data, res, cv::COLOR_BGR2GRAY); return res; } },
		{ "gaussian", [](cv::Mat data) { cv::Mat res; cv::GaussianBlur(data, res, cv::Size(3, 3), 0); return res; } },
		{ "circle", [](cv::Mat data) { cv::circle(data, cv::Point(300, 300), 3, cv::Scalar(1, 1, 1)); return data; } },

		{ "drawLogo", [logo = this->logo](cv::Mat data) {
			int x = data.cols - logo.cols;	// code from here: https://datahacker.rs/opencv-pixel-intensity-change-and-watermark/#id4
			int y = data.rows - logo.rows;
			cv::Mat imageROI = data(cv::Rect(x, y, logo.cols, logo.rows));
			cv::Mat invSrc = cv::Scalar::all(255) - logo;
			cv::Mat mask(invSrc);
			invSrc.copyTo(imageROI, mask);
			return data; } },

		{ "store1", [](cv::Mat data) { static uint64_t counter{}; cv::imwrite(std::filesystem::path{ "../../storeRes/store1/frame_" + std::to_string(++counter) + ".jpg" }.generic_string(), data); return data; } },
		{ "store2", [](cv::Mat data) { static uint64_t counter{}; cv::imwrite(std::filesystem::path{ "../../storeRes/store2/frame_" + std::to_string(++counter) + ".jpg" }.generic_string(), data); return data; } },
		{ "coords", [](cv::Mat data) { return data; } },
	};
};

// Used as a plug
// Designed to simplify the development and debugging process
struct OpencvLightChooser {

	OpencvLightChooser() {}
	OpencvLightChooser(cv::Mat logo) {}

	const std::vector<std::pair<std::string, std::function<cv::Mat(cv::Mat)>>> callables{
		{ "display", [](cv::Mat data) { return data; } },
		{ "rgb2gray", [](cv::Mat data) { return data; } },
		{ "gaussian", [](cv::Mat data) { return data; } },
		{ "circle", [](cv::Mat data) { return data; } },
		{ "drawLogo", [](cv::Mat data) { return data; } },
		{ "store1", [](cv::Mat data) { return data; } },
		{ "store2", [](cv::Mat data) { return data; } },
		{ "coords", [](cv::Mat data) { return data; } },
	};
};