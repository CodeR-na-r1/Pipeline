#include <iostream>
#include "opencv/opencv.hpp"
#include "pipeline/stage.hpp"
#include "parser/parser.hpp"

//#include <filesystem>

using namespace std;

using namespace Pipeline;

using namespace FakeOpenCV;

int main() {

	cout << "Test 1: construct pipeline" << endl;
	{
		Stage source = Stage{ std::function([](NDArray data) { return data; }), "SOURCE", {} };
		Stage blur = { std::function([](NDArray data) { return gaussian(data, 1.1); }), "BLUR", {} };
		Stage convert = { std::function([](NDArray data) { return rgb2gray(data, 13, 12.2); }), "CONVERT", {} };
		Stage binarize = { std::function([](NDArray data) { return FakeOpenCV::binarize(data); }), "BINARIZE", {} };
		Stage display = { std::function([](NDArray data) { FakeOpenCV::display(cout, data);  return data; }), "DISPLAY", {} };

		// in reverse order

		convert.addChild(binarize);
		convert.addChild(display);

		blur.addChild(convert);
		blur.addChild(display);

		source.addChild(blur);
		source.addChild(display);

		cout << source << endl;
		source(getImage());
	}

	cout << "Test 2: Configure pipeline by config from file" << endl;
	{

		ifstream fileJsonConfig("../pipeConfig.json");

		//cout << std::filesystem::current_path() << endl;
		//cout << filesystem::current_path();

		if (!fileJsonConfig.is_open()) {
			cerr << "File from object ifstream \'fileJsonConfig\' not open!" << endl;
			return -1;
		}

		auto source = JsonParser::fromFile<NDArray>(fileJsonConfig, [](std::string callableName) -> function<NDArray(NDArray)> {

			if (callableName == "getImage") {
				return [](NDArray data) { return data; };
			}
			else if (callableName == "rgb2gray") {
				return [](NDArray data) { return FakeOpenCV::rgb2gray(data, 13, 12.2); };
			}
			else if (callableName == "gaussian") {
				return [](NDArray data) { return FakeOpenCV::gaussian(data, 1.1); };
			}
			else if (callableName == "display") {
				return [](NDArray data) { FakeOpenCV::display(cout, data); return data; };
			}

			return [](NDArray data) { return data; };
			});

		cout << (*source) << endl;
		source->operator()(FakeOpenCV::getImage());
	}

	return 0;
}