#pragma once

#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>

#include "../detail/monitoring/IMeasurements.hpp"
#include "../connector/IConnector.hpp"
#include "../pipeline/Stage.hpp"

namespace Pipeline {

	namespace Stage {

		template <typename DataT, typename DaoT>
		struct IStageManager {

			virtual const std::vector<std::pair<std::size_t, std::function<void(void)>>>& getStageCallables() const = 0;

			virtual const std::unordered_map<std::size_t, Stage<DataT>>& getStageMap() const = 0;

			virtual const std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>>& getinputQMap() const = 0;

			virtual const std::unordered_map<std::size_t, std::vector<std::shared_ptr<Connector::IConnector<DaoT>>>>& getOutputQMap() const = 0;

			virtual const std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>>& getMeasurementMap() const = 0;
		};
	}
}
