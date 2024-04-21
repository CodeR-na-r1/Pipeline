#pragma once

#include "IInitializator.hpp"

#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>

namespace Benchmark {

	namespace Initializator {

		template<typename T>
		class XtensorInitializer : public IInitializer<T> {

			std::shared_ptr<xt::xarray<T>> data;
			xt::svector<std::size_t> shape;

		public:

			XtensorInitializer() : IInitializer<T>() {

				shape.reserve(3);
			}

			virtual void operator()(std::shared_ptr<zmq::message_t> msg, const NDArray::Reader& ndarray) override {

				// initialization code
				for (auto&& dimm : ndarray.getShape()) {
					shape.push_back(static_cast<std::size_t>(dimm));
				}

				auto tensorAdapter = xt::adapt_smart_ptr((T*)(&ndarray.getData().asBytes()[0]), shape, msg);
				data.reset(new xt::xarray<T>{ tensorAdapter });

				shape.clear();
			}

			virtual T getData(size_t idx) override {

				return data->at(idx);
			}
		};
	}
}