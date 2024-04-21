#pragma once

#include "IStageManager.hpp"

#include <memory>

#include <unordered_map>
#include <vector>

namespace Pipeline {

	namespace Stage {

		template <typename DataT, typename DaoT>
		class StageManager : public IStageManager<DataT, DaoT> {

			std::unordered_map<std::size_t, Stage<DataT>> stagesMap{}; // id ->  stage by id
			std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>> inputQueueMap{}; // get parent queue for current stage by id (i.e. get input queue)
			std::unordered_map<std::size_t, std::vector<std::shared_ptr<Connector::IConnector<DaoT>>>> outputQueuesMap{}; // get vector of child queues for current stage by id (i.e. get output queues)
			std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>> measurementsMap{};    // shared_ptr because mutex is not copyable

			std::vector<std::pair<std::size_t, std::function<void(void)>>> stageCallables{};

		public:

			StageManager() : IStageManager<DataT, DaoT>() {}

			StageManager(std::unordered_map<std::size_t, Stage<DataT>>&& _stagesMap,
				std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>> _inputQueueMap,
				std::unordered_map<std::size_t, std::vector<std::shared_ptr<Connector::IConnector<DaoT>>>> _outputQueuesMap,
				std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>> _measurementsMap,
				std::vector<std::pair<std::size_t, std::function<void(void)>>> _stageCallables)

				: IStageManager<DataT, DaoT>(), stagesMap(std::move(_stagesMap)), inputQueueMap(_inputQueueMap), outputQueuesMap(_outputQueuesMap), measurementsMap(_measurementsMap), stageCallables(_stageCallables) {}

			virtual const std::vector<std::pair<std::size_t, std::function<void(void)>>>& getStageCallables() const override {
				return stageCallables;
			}

			virtual const std::unordered_map<std::size_t, Stage<DataT>>& getStageMap() const override {
				return stagesMap;
			}

			virtual const std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>>& getinputQMap() const override {
				return inputQueueMap;
			}

			virtual const std::unordered_map<std::size_t, std::vector<std::shared_ptr<Connector::IConnector<DaoT>>>>& getOutputQMap() const override {
				return outputQueuesMap;
			}

			virtual const std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>>& getMeasurementMap() const override {
				return measurementsMap;
			}
		};
	}
}
