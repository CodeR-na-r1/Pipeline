#pragma once

#include <memory>

#include <atomic>
#include <chrono>

#include <functional>
#include <unordered_set>
#include <vector>

namespace Pipeline {

	namespace Builder {

		using namespace std::chrono_literals;

		struct ScalableManagerAssembly {

			/* service structures */

			std::unordered_set<std::size_t> scalableProperties{};
			std::shared_ptr<std::vector<std::pair<std::size_t, std::function<void(void)>>>> stageCallables{};

			std::shared_ptr<std::vector<std::atomic<std::function<void(void)>*>>> threadsManagement{};

			/* calculated fields */

			std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)> monitoringCallback{};
			std::vector<std::function<void(void)>> threadCallables{};

			/* public methods */

			ScalableManagerAssembly(const int ThreadsQuantity, const std::unordered_set<std::size_t>& scalableProperties, const std::vector<std::pair<std::size_t, std::function<void(void)>>>& stageCallables) : scalableProperties(scalableProperties) {

				threadsManagement.reset(new std::vector<std::atomic<std::function<void(void)>*>>{ static_cast<size_t>(ThreadsQuantity) });

				this->stageCallables.reset(new std::vector<std::pair<std::size_t, std::function<void(void)>>>{ stageCallables });
			}

			void initMonitoringCallback() {

				monitoringCallback = std::function<void(std::shared_ptr<detail::IMonitoringMeasurements>)>{ [scalableProperties = this->scalableProperties, stageCallables = this->stageCallables, threadsManagement = this->threadsManagement](std::shared_ptr<detail::IMonitoringMeasurements> arg) {

					int id{ -1 };
					std::int64_t max{ 0 };

					for (auto&& it : arg->getQueueLoad()) {

						if (scalableProperties.contains(it.first) && it.second > max) {
							max = it.second;
							id = it.first;
						}
					}

					if (id == -1) {

						for (auto& it : (*threadsManagement)) {

							it.store(nullptr, std::memory_order_release);
						}
						return;
					}

					std::vector<std::pair<std::size_t, std::function<void(void)>>>::iterator callableIt = std::find_if(stageCallables->begin(), stageCallables->end(), [id](auto&& value) { return value.first == id; });
					if (callableIt == stageCallables->end())
						return;

					std::function<void(void)>* callable = &(callableIt->second);

					for (auto& it : (*threadsManagement)) {

						it.store(callable, std::memory_order_release);
					}
				} };
			}

			void initThreadCallables() {

				threadCallables.clear();

				for (size_t i{ 0 }; i < threadsManagement->size(); ++i) {

					std::atomic<std::function<void(void)>*>* callable = &(*threadsManagement)[i];

					auto&& tempFunc = std::function<void(void)>{ [callable, threadsManagement = this->threadsManagement]() {

						if (auto functorP = callable->load(std::memory_order_relaxed)) {
							
							(*functorP)();
						}
						else {
							std::this_thread::sleep_for(100ms);
						}
					} };

					threadCallables.push_back(std::move(tempFunc));
				}
			}

		};
	}
}