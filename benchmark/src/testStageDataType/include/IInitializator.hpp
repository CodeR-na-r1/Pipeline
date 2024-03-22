#pragma once

#include <memory>

#include "../scheme/ndarray.capnp.h"
#include "zmq.hpp"

namespace Benchmark {

	namespace Initializator {

		template<typename T>
		class IInitializer {

		public:

			/* return its running time in milliseconds */
			double bench(std::shared_ptr<zmq::message_t> msg, const NDArray::Reader& ndarray) {

				auto startTimer = std::chrono::high_resolution_clock::now();

				this->operator()(msg, ndarray);
				
				auto finishTimer = std::chrono::high_resolution_clock::now();
				auto time = std::chrono::duration<double, std::milli>(finishTimer - startTimer).count();

				return time;
			}

			virtual void operator()(std::shared_ptr<zmq::message_t> msg, const NDArray::Reader& ndarray) = 0;

			virtual T getData(size_t idx) = 0;
		};
	}
}