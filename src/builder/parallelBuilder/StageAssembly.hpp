#pragma once

#include <chrono>
#include <memory>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>

#include "../../pipeline/Stage.hpp"
#include "../../connector/SPSCConnector.hpp"
#include "../../detail/monitoring/Measurements.hpp"

namespace Pipeline {

	namespace Builder {

		using namespace std::chrono_literals;

		template <typename DataT, typename DaoT>
		struct StageAssembly {

			std::unordered_map<std::size_t, Stage::Stage<DataT>> stagesMap{}; // id ->  stage by id
			std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>> inputQueueMap{}; // get parent queue for current stage by id (i.e. get input queue)
			std::unordered_map<std::size_t, std::vector<std::shared_ptr<Connector::IConnector<DaoT>>>> outputQueuesMap{}; // get vector of child queues for current stage by id (i.e. get output queues)
			std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>> measurementsMap{};    // shared_ptr because mutex is not copyable

			std::vector<std::pair<std::size_t, std::function<void(void)>>> stageCallables{};

			StageAssembly() = default;

			const std::vector<std::pair<std::size_t, std::function<void(void)>>>& getStageCallables() {

				return stageCallables;
			}

			void initStages(Stage::Stage<DataT>&& stages) {

				// reset
				stagesMap.clear();
				inputQueueMap.clear();
				outputQueuesMap.clear();
				measurementsMap.clear();

				// fill

				inputQueueMap.insert({ 0, std::shared_ptr<Connector::IConnector<DaoT>>{new Connector::SPSCConnector<DaoT, 1024>{}} });

				std::queue<Stage::Stage<DataT>*> queue;
				queue.push(&stages);

				while (!queue.empty()) {

					auto current = queue.front();

					measurementsMap.insert({ current->getId(), std::shared_ptr<detail::IMeasurements>{new detail::Measurements{}} });

					outputQueuesMap.insert({ current->getId(), {} });
					outputQueuesMap.at(current->getId()).reserve(current->getChilds().size());

					for (auto&& child : current->getChilds()) {

						std::shared_ptr<Connector::IConnector<DaoT>> temp{ new Connector::SPSCConnector<DaoT, 1024>{} };
						outputQueuesMap.at(current->getId()).push_back(temp);
						inputQueueMap.insert({ child.getId(), temp });
						queue.push(&child);
					}

					stagesMap.insert({ current->getId(), std::move(*current) });

					queue.pop();
				}
			}

			void initCallables(std::function<DataT(DaoT)> mapperFromDao, std::function<DaoT(DaoT, DataT)> mapperToDao) {

				// callables reset
				stageCallables.clear();

				// put stage in vector
				for (auto&& stageIt = stagesMap.begin(); stageIt != stagesMap.end(); ++stageIt) {

					// preparing the necessary objects
					auto&& inputQueue = inputQueueMap.at(stageIt->second.getId());
					auto&& outputQueues = outputQueuesMap.at(stageIt->second.getId());
					auto&& executor = stageIt->second.getCallable();
					auto&& measurements = measurementsMap.at(stageIt->second.getId());

					// create function
					std::function<void(void)> callable = [id = stageIt->first, inputQueue, outputQueues, executor, measurements, mapperFromDao, mapperToDao]() {

						auto&& startTimer = std::chrono::high_resolution_clock::now();

						if (inputQueue->hasValue()) {

							DaoT argData;
							if (inputQueue->extract(argData)) { // else queue is empty (another thread may have received the value earlier)

								auto&& res = mapperToDao(argData, executor(mapperFromDao(argData)));
								for (auto&& q : outputQueues) {

									while (!q->push(res)) {}
								}

								auto&& finishTimer = std::chrono::high_resolution_clock::now();

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
}