#pragma once

#include "../detail/monitoring/IMonitoringMeasurements.hpp"

#include <memory>
#include <vector>
#include <functional>

namespace Pipeline {

	namespace Scalable {

		struct IScalableManager {

			virtual const std::function<void(std::shared_ptr< detail::IMonitoringMeasurements>)>& getMonitoringCallback() = 0;

			virtual const std::vector<std::function<void(void)>>& getThreadCallables() = 0;
			
			virtual ~IScalableManager() = default;
		};
	}
}
