#pragma once

namespace Pipeline {

	namespace Proto {

		template<typename DataT, typename BrokerT>
		struct ISerializationManager {

			virtual BrokerT operator()(DataT) = 0;

			virtual ~ISerializationManager() = default;
		};
	}
}