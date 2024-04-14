#pragma once

#include <barrier>
#include <memory>
#include "chrono"

#include <unordered_map>
#include <vector>
#include <functional>

#include <type_traits>

#include "../detail/monitoring/IMeasurements.hpp"
#include "../connector/IConnector.hpp"
#include "../detail/monitoring/MonitoringMeasurements.hpp"

namespace Pipeline {

	namespace Builder {

		using namespace std::chrono_literals;

		template <typename DaoT>
		struct MonitoringAssembly {

			[[nodiscard]]
			std::pair<std::vector<std::function<void(void)>>, std::function<void(void)>> getCallables(const std::vector<std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)>> monitoringCallbacks,
				const std::unordered_map<std::size_t, std::shared_ptr<Connector::IConnector<DaoT>>>& queuesMap,
				const std::unordered_map<std::size_t, std::shared_ptr<detail::IMeasurements>>& measurementsMap) {

				std::shared_ptr<std::atomic_bool> isEndPhase{ new std::atomic_bool{} };
				auto completeFunc = [isEndPhase]() noexcept { isEndPhase->store(true, std::memory_order_relaxed); };

				auto barrierRawPointer = new std::barrier{ std::ssize(monitoringCallbacks) + 1, completeFunc };
				std::shared_ptr<std::remove_pointer<decltype(barrierRawPointer)>::type> barrier{ barrierRawPointer };

				std::unordered_map<std::size_t, std::size_t> queueLoad{};
				std::unordered_map<std::size_t, double> avgTimePerCallable{};
				for (auto&& it : queuesMap) {

					queueLoad.insert({ it.first, 0 });
					avgTimePerCallable.insert({ it.first, 0.0 });
				}

				auto&& rawPointer = new detail::MonitoringMeasurements{ queueLoad, avgTimePerCallable };
				std::shared_ptr<detail::IMonitoringMeasurements> mMeasurements{ rawPointer };

				std::vector<std::function<void(void)>> resCallbacks{};

				// main monitoring thread
				auto callable = std::function<void(void)>{ [queuesMap, measurementsMap, rawPointer, barrier]() {

					for (auto&& it : queuesMap) {

						rawPointer->queueLoad.at(it.first) = it.second->size();
						rawPointer->avgTimePerCallable.at(it.first) = measurementsMap.at(it.first)->pull();
					}

					barrier->arrive_and_wait();

					std::this_thread::sleep_for(3000ms);
				} };
				resCallbacks.push_back(std::move(callable));

				// monitoring subscribers threads
				for (auto it : monitoringCallbacks) {

					callable = std::function<void(void)>{ [it, mMeasurements, barrier]() {

						barrier->arrive_and_wait();

						it(mMeasurements);
					} };
					resCallbacks.push_back(std::move(callable));
				}

				auto stopFunction = std::function<void(void)>{ [isEndPhase, barrier]() {

					isEndPhase->store(false, std::memory_order_release);

					while (!isEndPhase->load(std::memory_order_acquire)) {

						barrier->arrive();
					}
					}
				};

				return { resCallbacks, stopFunction };
			}
		};
	}
}