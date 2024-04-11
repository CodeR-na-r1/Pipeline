#pragma once

#include <memory>
#include <functional>

#include "../NetworkManager/IReceiverNetworkManager.hpp"
#include "../connector/IConnector.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT, typename DaoT>
		struct IReceiverNetworkManagerBuilder {

			virtual std::shared_ptr<Network::IReceiverNetworkManager> build(std::shared_ptr<Connector::IConnector<DaoT>>, std::function<DaoT(DataT)>) = 0;
		};
	}
}