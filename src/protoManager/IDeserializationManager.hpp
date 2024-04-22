#pragma once

#include <memory> // shared_ptr

#include "zmq.hpp"	// message_t

namespace Pipeline {

	namespace Proto {

		template<typename BrokerT, typename DataT>
		struct IDeserializationManager {

			virtual DataT operator()(BrokerT) = 0;
			
			virtual ~IDeserializationManager() = default;
		};
	}
}
