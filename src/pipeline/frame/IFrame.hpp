#pragma once

namespace Pipeline {

	namespace Frame {

		template <typename DataT>
		struct IFrame {

			virtual DataT getData() = 0;
		};
	}
}