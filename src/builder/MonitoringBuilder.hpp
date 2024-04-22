#pragma once

#include "IMonitoringBuilder.hpp"

#include <functional>
#include <vector>

#include "../detail/monitoring/IMonitoringMeasurements.hpp"
#include "MonitoringAssembly.hpp"
#include "../monitoringManager/MonitoringManager.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DaoT>
		class MonitoringBuilder : public IMonitoringBuilder<DaoT> {

			MonitoringAssembly<DaoT> mAsm{};

		public:

			MonitoringBuilder() : IMonitoringBuilder<DaoT>() {}

			virtual MonitoringBuilder& addMonitoringCallback(const std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)> monitoringCallback) override {

				mAsm.monitoringCallbacks.push_back(monitoringCallback);

				return *this;
			}

			[[nodiscard]]
			std::unique_ptr<IMonitoringBuilder<DaoT>> build() {

				return std::unique_ptr<IMonitoringBuilder<DaoT>>{new MonitoringBuilder<DaoT>{ std::move(*this) }};
			}

			[[nodiscard]]
			virtual std::shared_ptr<Monitoring::IMonitoringManager> build(const std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>>& queuesMap, const std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>>& measurementsMap) override {

				auto&& [resCallables, stopFunc] = mAsm.getCallables(queuesMap, measurementsMap);

				return std::shared_ptr<Monitoring::IMonitoringManager>{new Monitoring::MonitoringManager{ resCallables, stopFunc }};
			}
		};
	}
}