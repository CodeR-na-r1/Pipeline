#pragma once

namespace Pipeline {

	namespace Network {

		struct IReceiverNetworkManager {

			virtual void operator()() = 0;
			
			virtual ~IReceiverNetworkManager() = default;
		};
	}
}
