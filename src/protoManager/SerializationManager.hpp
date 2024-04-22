#pragma once

#include "ISerializationManager.hpp"

#include <memory> // unique_ptr

#include "zmq.hpp"	// message_t

#include "scheme/ndarray.capnp.h"	// NDArray
#include "capnp/serialize.h"	// FlatArrayMessageReader, etc ...

#include "opencv2/opencv.hpp"	// cv::Mat

namespace Pipeline {

	namespace Proto {

		template<typename DataT> class NDArraySerializator {};	// primary template

		template<> class NDArraySerializator<cv::Mat> : public ISerializationManager<cv::Mat, zmq::message_t> {	// spec for cv::Mat

			cv::Size shape{};

		public:

			NDArraySerializator() = default;
			NDArraySerializator(const NDArraySerializator&) = default;
			NDArraySerializator(NDArraySerializator&&) = default;

			[[nodiscard]]
			virtual zmq::message_t operator()(cv::Mat arg) override {

				// serialization
				// 
				//create message
				capnp::MallocMessageBuilder message;
				NDArray::Builder ndarray = message.initRoot<NDArray>();

				// fill fields
				// shape
				auto&& shape = ndarray.initShape(2);
				shape.set(0, arg.size().height);
				shape.set(1, arg.size().width);
				// data
				kj::ArrayPtr<const kj::byte> data{ arg.data, arg.total() * arg.elemSize() };
				ndarray.setData(data);
				ndarray.setDtype(getDType(arg));

				kj::Array<capnp::word> serialized_message = capnp::messageToFlatArray(message);

				return { reinterpret_cast<void*>(serialized_message.begin()), serialized_message.size() * sizeof(capnp::word) };
			}

			decltype(NDArray::DType::UINT8) getDType(const cv::Mat& arg) {

				switch (arg.type()) {

				case CV_8UC3:
					return NDArray::DType::UINT8;

				case CV_16U:
					return NDArray::DType::UINT16;

				case CV_32S:
					return NDArray::DType::UINT32;

				case CV_8S:
					return NDArray::DType::INT8;

				case CV_16S:
					return NDArray::DType::INT16;

				case CV_32F:
					return NDArray::DType::FLOAT32;

				case CV_64F:
					return NDArray::DType::FLOAT64;
				}

				return NDArray::DType::UINT8;
				throw std::runtime_error{ "Match not found for dType" };
			}

			std::shared_ptr<ISerializationManager> build() {

				return std::shared_ptr<ISerializationManager>{ new NDArraySerializator<cv::Mat>{ std::move(*this) } };
			}

			virtual ~NDArraySerializator() = default;
		};
	}
}