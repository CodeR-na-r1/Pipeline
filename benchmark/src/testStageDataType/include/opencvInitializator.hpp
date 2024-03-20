#pragma once

#include "IInitializator.hpp"

#include "opencv2/opencv.hpp"

namespace Benchmark {

	namespace Initializator {

		template<typename T>
		class OpencvInitializer : public IInitializer<T> {

			//fields

		public:

			OpencvInitializer() : IInitializer<T>() {


			}

			virtual void operator()(std::shared_ptr<zmq::message_t> msg, NDArray::Reader& ndarray) override {

				// initialization code
			}

			virtual T getData(size_t idx) override {

				return T{};
			}
		};
	}
}