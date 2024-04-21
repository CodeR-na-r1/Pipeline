#pragma once

#include <stdexcept>

#include "IInitializator.hpp"

#include "opencv2/opencv.hpp"
#include <opencv2/core/hal/interface.h>

namespace Benchmark {

	namespace Initializator {

		template<typename T>
		class OpencvInitializer : public IInitializer<T> {

			cv::Mat data;
			cv::Size shape;
			int dType;

			std::shared_ptr<zmq::message_t> dataOrig;

		public:

			OpencvInitializer() : IInitializer<T>() {


			}

			virtual void operator()(std::shared_ptr<zmq::message_t> msg, const NDArray::Reader& ndarray) override {

				// initialization code
				assert(ndarray.getShape().size() == 2 && "Shape for opencv::Mat not correct");	// compile-time check

				if (ndarray.getShape().size() != 2)	// runtime check for release mode
					throw std::runtime_error{ "Shape for opencv::Mat not correct" };

				shape.width = ndarray.getShape()[0];
				shape.height = ndarray.getShape()[1];

				dType = getDType(ndarray.getDtype());
				
				dataOrig = msg;	// save ptr, because cv::Mat not copy data

				data = cv::Mat(shape, dType, (void*)(&ndarray.getData().asBytes()[0])).clone();	// clone for deep copy
			}

			virtual T getData(size_t idx) override {

				return data.at<T>(idx);
			}

			auto getDType(const NDArray::DType& dType) {

				switch (dType) {

				case NDArray::DType::UINT8:
					return CV_8U;

				case NDArray::DType::UINT16:
					return CV_16U;

				case NDArray::DType::UINT32:
					return CV_32S;

				case NDArray::DType::INT8:
					return CV_8S;

				case NDArray::DType::INT16:
					return CV_16S;

				case NDArray::DType::INT32:
					return CV_32S;

				case NDArray::DType::FLOAT32:
					return CV_32F;

				case NDArray::DType::FLOAT64:
					return CV_64F;
				}

				throw std::runtime_error{ "Match not found for dType" };
			}
		};
	}
}