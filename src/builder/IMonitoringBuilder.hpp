#pragma once

#include "../MonitoringManager/IMonitoringManager.hpp"

#include <memory>
#include <unordered_map>

#include "../connector/IConnector.hpp"
#include "../detail/monitoring/IMeasurements.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DaoT>
		struct IMonitoringBuilder {

			virtual std::shared_ptr<Monitoring::IMonitoringManager> build(const std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>>&, const std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>>&) = 0;
		};
	}
}