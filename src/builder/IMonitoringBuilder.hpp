#pragma once

#include "../monitoringManager/IMonitoringManager.hpp"

#include <memory>
#include <functional>
#include <unordered_map>

#include "../connector/IConnector.hpp"
#include "../detail/monitoring/IMeasurements.hpp"
#include "../detail/monitoring/IMonitoringMeasurements.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DaoT>
		struct IMonitoringBuilder {

			virtual IMonitoringBuilder& addMonitoringCallback(const std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)>) = 0;

			virtual std::shared_ptr<Monitoring::IMonitoringManager> build(const std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>>&, const std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>>&) = 0;
			
			virtual ~IMonitoringBuilder() = default;
		};
	}
}
