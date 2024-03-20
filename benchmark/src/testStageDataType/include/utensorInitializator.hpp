#pragma once

#include "IInitializator.hpp"

//headers

namespace Benchmark {

	namespace Initializator {

		template<typename T>
		class UtensorInitializer : public IInitializer<T> {

			//fields

		public:

			UtensorInitializer() : IInitializer<T>() {


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