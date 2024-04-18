#pragma once

#include <memory>
#include <vector>

#include "../../NetworkManager/IReceiverNetworkManager.hpp"
#include "../../StageManager/IStageManager.hpp"
#include "../../MonitoringManager/IMonitoringManager.hpp"
#include "../../NetworkManager/ISendlerNetworkManager.hpp"

#include "../../ScalableManager/IScalableManager.hpp"

#include "../../DLManager/IDLManager.hpp"

namespace Pipeline {

	namespace detail {

		template <typename DataT, typename DaoT>
		struct ScalableParallelPipelineComponents {

			std::shared_ptr<DL::IDLManager> dlM{};

			std::vector<std::shared_ptr<Network::IReceiverNetworkManager>> rNetworkM{};
			std::shared_ptr<Stage::IStageManager<DataT, DaoT>> stagesM{};
			std::shared_ptr<Monitoring::IMonitoringManager> monitoringM{};
			std::vector<std::shared_ptr<Network::ISendlerNetworkManager>> sNetworkM{};

			std::shared_ptr<Scalable::IScalableManager> scalableM{};
		};
	}
}