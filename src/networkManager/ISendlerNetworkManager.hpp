#pragma once

namespace Pipeline {

	namespace Network {

		struct ISendlerNetworkManager {

			virtual void operator()() = 0;
			
			virtual ~ISendlerNetworkManager() = default;
		};
	}
}
