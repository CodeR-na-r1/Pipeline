#pragma once
/* file for emulating opencv functions  */

#include <iostream>
using std::ostream;

namespace FakeOpenCV {

	struct NDArray {

		int shape;
		int dtype;
		int timestamp;
		int data;

		int getData() const {
			return data;
		}
	};

	NDArray getImage() {

		return NDArray(2, 1, 0, 1000);
	}

	NDArray gaussian(NDArray image, double sigma) {

		return NDArray(image.shape, image.dtype, image.timestamp +1, image.data * sigma);
	}

	NDArray binarize(NDArray image) {
		return NDArray(image.shape, image.dtype, image.timestamp +1, 1010101001);
	}

	NDArray rgb2gray(NDArray image, int someParameter, double someParameter2) {

		return NDArray(1, image.dtype, image.timestamp +1, 127);
	}

	void display(ostream& os, NDArray data) {

		os << "Some display data -> " << data.data << std::endl;
	}
}