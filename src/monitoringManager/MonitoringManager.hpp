#pragma once

#include "IMonitoringManager.hpp"

namespace Pipeline {

	namespace Monitoring {

		class MonitoringManager : public IMonitoringManager {

			std::vector<std::function<void(void)>> callables;
			std::function<void(void)> stopFunction{};

		public:

			MonitoringManager(std::vector<std::function<void(void)>> callables, std::function<void(void)> stopFunction) : IMonitoringManager(), callables(callables), stopFunction(stopFunction) {}

			virtual const std::vector<std::function<void(void)>>& getCallables() override {

				return callables;
			}

			virtual const std::function<void(void)>& getStopFunction() override {

				return stopFunction;
			}
		};
	}
}