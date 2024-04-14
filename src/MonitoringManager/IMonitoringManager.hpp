#pragma once

#include <vector>
#include "functional"

namespace Pipeline {

	namespace Monitoring {

		struct IMonitoringManager {

			virtual const std::vector<std::function<void(void)>>& getCallables() = 0;

			virtual const std::function<void(void)>& getStopFunction() = 0;
		};
	}
}