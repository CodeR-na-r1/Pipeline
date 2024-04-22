#pragma once

#include <unordered_map>

namespace Pipeline {

	namespace detail {

		struct IMonitoringMeasurements {

			virtual const std::unordered_map<std::size_t, std::size_t>& getQueueLoad() = 0;

			virtual const std::unordered_map<std::size_t, double>& getAvgTimePerCallable() = 0;
			
			virtual ~IMonitoringMeasurements() = default;
		};
	}
}
