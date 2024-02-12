#include <iostream>
#include "conveyor/conveyor.hpp"
#include "opencv/opencv.hpp"

using namespace std;

using namespace Conveyor;

using namespace FakeOpenCV;

int main() {

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

	return 0;
}