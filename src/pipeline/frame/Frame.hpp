#pragma once

#include "IFrame.hpp"

namespace Pipeline {

	namespace Frame {

		template <typename DataT>
		class Frame : public IFrame<DataT> {

			DataT data{};

		public:

			Frame() = default;

			Frame(const DataT& data) : IFrame<DataT>(), data(data) {}

			virtual DataT getData() override {
				return data;
			}
		};
	}
}