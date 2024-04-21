#pragma once

#include "IDeserializationManager.hpp"

#include <memory> // shared_ptr

#include "zmq.hpp"	// message_t

#include "scheme/ndarray.capnp.h"	// NDArray
#include "capnp/serialize.h"	// FlatArrayMessageReader, etc ...

#include "opencv2/opencv.hpp"	// cv::Mat

#include "xtensor/xarray.hpp"	// xarray
#include "xtensor/xadapt.hpp"	// adapt_smart_ptr

namespace Pipeline {

	namespace Proto {

		template<typename DataT> class NDArrayDeserializator {};	// primary template

		template<> class NDArrayDeserializator<cv::Mat> : public IDeserializationManager<std::shared_ptr<zmq::message_t>, cv::Mat> {	// spec for cv::Mat

			cv::Size shape;

		public:

			[[nodiscard]]
			virtual cv::Mat operator()(std::shared_ptr<zmq::message_t> msg) override {

				// create a pointer to aligned data from the received message
				auto alignedPtr = kj::ArrayPtr<const capnp::word>((capnp::word*)msg->data(), msg->size() / sizeof(capnp::word));

				// create an input stream from the memory buffer
				capnp::FlatArrayMessageReader message_reader(alignedPtr, capnp::ReaderOptions{ static_cast<std::uint64_t>(9999999999999), 512 });
				NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

				// initialization code
				assert((ndarray.getShape().size() == 2 ||
					(ndarray.getShape().size() == 3 && ndarray.getDtype() == NDArray::DType::UINT8)) &&
					"Shape for opencv::Mat not correct");	// compile-time check

				if (!(ndarray.getShape().size() == 2 ||
					(ndarray.getShape().size() == 3 && ndarray.getDtype() == NDArray::DType::UINT8)))	// runtime check for release mode
					throw std::runtime_error{ "Shape for opencv::Mat not correct" };

				shape.width = ndarray.getShape()[1];
				shape.height = ndarray.getShape()[0];

				auto dType = getDType(ndarray.getDtype());

				auto data = cv::Mat(shape, dType, (void*)(&ndarray.getData().asBytes()[0])).clone();	// clone for deep copy

				return data;
			}

			decltype(CV_8UC3) getDType(const NDArray::DType& dType) {

				switch (dType) {

				case NDArray::DType::UINT8:
					return CV_8UC3;

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

			std::unique_ptr<IDeserializationManager> build() {

				return std::unique_ptr<IDeserializationManager>{ new NDArrayDeserializator<cv::Mat>{ std::move(*this) } };
			}
		};

		template <typename DataT>
		class NDArrayDeserializator<xt::xarray<DataT>> : public IDeserializationManager<std::shared_ptr<zmq::message_t>, std::shared_ptr<xt::xarray<DataT>>> {	// spec for xt::xarray

			using TensorT = xt::xarray<DataT>;

			xt::svector<size_t> shape;

		public:

			NDArrayDeserializator() {

				shape.reserve(3);
			}

			[[nodiscard]]
			virtual std::shared_ptr<TensorT> operator()(std::shared_ptr<zmq::message_t> msg) override {

				// create a pointer to aligned data from the received message
				auto alignedPtr = kj::ArrayPtr<const capnp::word>((capnp::word*)msg->data(), msg->size() / sizeof(capnp::word));

				// create an input stream from the memory buffer
				capnp::FlatArrayMessageReader message_reader(alignedPtr, capnp::ReaderOptions{ static_cast<std::uint64_t>(9999999999999), 512 });
				NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

				// shape
				for (auto&& dimm : ndarray.getShape()) {
					shape.push_back(static_cast<std::size_t>(dimm));
				}

				// data
				auto tensorAdapter = xt::adapt_smart_ptr((DataT*)(&ndarray.getData().asBytes()[0]), shape, msg);
				std::shared_ptr<TensorT> tensor{ new TensorT { tensorAdapter} };
				shape.clear();

				return tensor;
			}
		};
	}
}
