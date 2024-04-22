#pragma once

#include "IFrame.hpp"

namespace Pipeline {

	namespace Frame {

		template <typename IDType, typename DataT>
		struct IIDFrame : public IFrame<DataT> {

			IIDFrame() :IFrame<DataT>() {}

			virtual const IDType getID() const = 0;

			using IFrame<DataT>::getData;
			
			virtual ~IIDFrame() = default;
		};
	}
}
