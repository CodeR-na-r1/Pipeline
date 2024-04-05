#pragma once

#include "IIDFrame.hpp"

namespace Pipeline {

	namespace Frame {

		template <typename IDType, typename DataT>
		class IDFrame : public IIDFrame<IDType, DataT> {

			IDType id;
			DataT data;

		public:

			IDFrame(const IDType& id, const DataT& data) : IIDFrame<IDType, DataT>(), id(id), data(data) {}

			virtual const IDType getID() const override {
				return id;
			}

			virtual DataT getData() override {
				return data;
			}
		};
	}
}