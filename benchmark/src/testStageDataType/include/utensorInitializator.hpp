#pragma once

#include "IInitializator.hpp"

#include <memory>
#include <vector>

#include "boost/numeric/ublas/tensor/tensor.hpp"
#include <boost/numeric/ublas/tensor/extents.hpp>   // extents == shape

namespace Benchmark {

	namespace Initializator {

		template<typename T>
		class UtensorInitializer : public IInitializer<T> {

			using DataT = std::shared_ptr<boost::numeric::ublas::tensor<T>>;
			using TensorT = boost::numeric::ublas::tensor<T>;
			using ShapeT = boost::numeric::ublas::shape;

			DataT data;
			
			std::vector<std::size_t> shapeVector;

		public:

			UtensorInitializer() : IInitializer<T>() {
				
				shapeVector.reserve(3);
			}

			virtual void operator()(std::shared_ptr<zmq::message_t> msg, NDArray::Reader& ndarray) override {

				if (ndarray.getShape().size() == 1)
					shapeVector.push_back(1);

				for (auto&& dimm : ndarray.getShape()) {
					shapeVector.push_back(static_cast<std::size_t>(dimm));
				}
				ShapeT shapeTensor{ {shapeVector.crbegin(), shapeVector.crend()} };
				shapeVector.clear();

				data.reset(new TensorT{ shapeTensor });

				auto ndarrayData = (T*)ndarray.getData().begin();
				for (size_t i(0ul); i < data->extents().product(); ++i)
				{
					(*data)[i] = *(ndarrayData++);  // I have not been able to use std::copy here :(
				}
			}

			virtual T getData(size_t idx) override {

				return data->operator[](idx);
			}
		};
	}
}