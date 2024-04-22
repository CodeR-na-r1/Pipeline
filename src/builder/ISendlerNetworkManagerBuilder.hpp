#pragma once

#include <string>
#include <memory>
#include <functional>

#include "../networkManager/ISendlerNetworkManager.hpp"
#include "../connector/IConnector.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT, typename DaoT>
		struct ISendlerNetworkManagerBuilder {

			virtual const std::string& getStageConnectName() = 0;

			virtual std::shared_ptr<Network::ISendlerNetworkManager> build(std::shared_ptr<Connector::IConnector<DaoT>> queue, std::function<DataT(DaoT)> mapper) = 0;
			
			virtual ~ISendlerNetworkManagerBuilder() = default;
		};
	}
}