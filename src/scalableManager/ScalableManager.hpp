#pragma once

#include "IScalableManager.hpp"

#include <atomic>
#include <chrono>

#include <unordered_set>
#include <array>

namespace Pipeline {

	namespace Scalable {

		class ScalableManager : public IScalableManager {

			std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)> monitoringCallback{};
			std::vector<std::function<void(void)>> threadCallables{};

		public:

			ScalableManager(std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)> monitoringCallback, std::vector<std::function<void(void)>> threadCallables) : IScalableManager(), monitoringCallback(monitoringCallback), threadCallables(threadCallables) {}

			virtual const std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)>& getMonitoringCallback() override {

				return monitoringCallback;
			}

			virtual const std::vector<std::function<void(void)>>& getThreadCallables() override {

				return threadCallables;
			}
		};
	}
}