#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "../../pipeline/Stage.hpp"

#include "../../connector/SPSCConnector.hpp"
#include "../../connector/MPMCConnector.hpp"
#include "../../connector/SyncMPMCConnector.hpp"

#include "../../detail/monitoring/Measurements.hpp"

namespace Pipeline {

	namespace Builder {

		using namespace std::chrono_literals;

		template <typename DataT, typename DaoT>
		struct ScalableStageAssembly {

			std::unordered_map<std::size_t, Stage::Stage<DataT>> stagesMap{}; // id ->  stage by id
			std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>> inputQueueMap{}; // get parent queue for current stage by id (i.e. get input queue)
			std::unordered_map<std::size_t, std::vector<std::shared_ptr<Connector::IConnector<DaoT>>>> outputQueuesMap{}; // get vector of child queues for current stage by id (i.e. get output queues)
			std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>> measurementsMap{};    // shared_ptr because mutex is not copyable

			std::vector<std::pair<std::size_t, std::function<void(void)>>> stageCallables{};

			ScalableStageAssembly() = default;

			const std::vector<std::pair<std::size_t, std::function<void(void)>>>& getStageCallables() {

				return stageCallables;
			}

			void initStages(const Stage::Stage<DataT>& stages, const std::unordered_set<std::size_t>& scalableProperties, const std::unordered_set<std::size_t>& synchronyzeProperties, bool isOneRNManager, std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>>& outputQueues) {

				// reset
				stagesMap.clear();
				inputQueueMap.clear();
				outputQueuesMap.clear();
				measurementsMap.clear();

				// fill

				if (synchronyzeProperties.contains(stages.getId()))
					inputQueueMap.insert({ stages.getId(), std::shared_ptr<Connector::IConnector<DaoT>>{new Connector::SyncMapConnector<DaoT, 1024>{}} });
				else if (scalableProperties.contains(stages.getId()) || !isOneRNManager)
					inputQueueMap.insert({ stages.getId(), std::shared_ptr<Connector::IConnector<DaoT>>{new Connector::MPMCConnector<DaoT, 1024>{}} });
				else
					inputQueueMap.insert({ stages.getId(), std::shared_ptr<Connector::IConnector<DaoT>>{new Connector::SPSCConnector<DaoT, 1024>{}} });

				std::queue<const Stage::Stage<DataT>*> queue;
				queue.push(&stages);

				while (!queue.empty()) {

					auto current = queue.front();

					measurementsMap.insert({ current->getId(), std::shared_ptr<detail::IMeasurements>{new detail::Measurements{}} });

					outputQueuesMap.insert({ current->getId(), {} });
					outputQueuesMap.at(current->getId()).reserve(current->getChilds().size());

					for (auto&& child : current->getChilds()) {

						auto&& temp = std::shared_ptr<Connector::IConnector<DaoT>>{};

						if (synchronyzeProperties.contains(child.getId()))
							temp.reset(new Connector::SyncMapConnector<DaoT, 1024>{});
						else if (scalableProperties.contains(current->getId()) || scalableProperties.contains(child.getId()))
							temp.reset(new Connector::MPMCConnector<DaoT, 1024>{});
						else
							temp.reset(new Connector::SPSCConnector<DaoT, 1024>{});

						outputQueuesMap.at(current->getId()).push_back(temp);
						inputQueueMap.insert({ child.getId(), temp });
						queue.push(&child);
					}

					stagesMap.insert({ current->getId(), std::move(*current) });

					queue.pop();
				}

				for (auto&& it : outputQueues) {

					auto&& stageName = it.first;
					auto&& idOpt = detail::nameToId(stages, stageName);

					if (!idOpt.has_value())
						throw std::runtime_error{ std::string{"Stage with name \'"} + stageName + std::string{"\' not found"} };

					auto&& id = idOpt.value();

					auto&& temp = std::shared_ptr<Connector::IConnector<DaoT>>{};

					if (scalableProperties.contains(id))
						temp.reset(new Connector::MPMCConnector<DaoT, 1024>{});
					else
						temp.reset(new Connector::SPSCConnector<DaoT, 1024>{});

					outputQueuesMap[id].push_back(temp);
					it.second = temp;
				}
			}

			void initCallables(std::function<DataT(DaoT)> mapperFromDao, std::function<DaoT(DaoT, DataT)> mapperToDao) {

				// callables reset
				stageCallables.clear();

				// put stage in vector
				for (auto stageIt = stagesMap.begin(); stageIt != stagesMap.end(); ++stageIt) {

					// preparing the necessary objects
					auto&& inputQueue = inputQueueMap.at(stageIt->second.getId());
					auto&& outputQueues = outputQueuesMap.at(stageIt->second.getId());
					auto&& executor = stageIt->second.getCallable();
					auto&& measurements = measurementsMap.at(stageIt->second.getId());

					// create function
					std::function<void(void)> callable = [id = stageIt->first, inputQueue, outputQueues, executor, measurements, mapperFromDao, mapperToDao]() {

						auto startTimer = std::chrono::high_resolution_clock::now();

						if (inputQueue->hasValue()) {

							DaoT argData;
							if (inputQueue->extract(argData)) { // else queue is empty (another thread may have received the value earlier)

								auto&& res = mapperToDao(argData, executor(mapperFromDao(argData)));
								for (auto&& q : outputQueues) {

									while (!q->push(res)) {}
								}

								auto finishTimer = std::chrono::high_resolution_clock::now();

								measurements->push(std::chrono::duration<double, std::milli>(finishTimer - startTimer).count());
							}
						}
						else
							std::this_thread::sleep_for(10ms);
					};

					stageCallables.emplace_back(stageIt->first, std::move(callable));
				}
			}
		};
	}
};