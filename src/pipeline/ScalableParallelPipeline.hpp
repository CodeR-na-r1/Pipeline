#pragma once

#include "../detail/pipeline/ScalableParallelPipelineComponents.hpp"

#include "boost/thread/thread.hpp"

namespace Pipeline {

	template <typename DataT, typename DaoT>
	class ScalableParallelPipeline {

		detail::ScalableParallelPipelineComponents<DataT, DaoT> components{};

		boost::thread_group rNetworkMTPool;
		boost::thread_group stagesMTPool;
		boost::thread_group monitoringMTPool;
		boost::thread_group sNetworkMTPool;
		boost::thread_group scalableMTPool;

	public:

		ScalableParallelPipeline(detail::ScalableParallelPipelineComponents<DataT, DaoT> components) : components(components) {}

		void start() {

			for (auto&& it : components.rNetworkM) {

				rNetworkMTPool.create_thread([rNM = it]() {

					while (true) {

						boost::this_thread::interruption_point();

						rNM->operator()();
					}
				});
			}

			for (auto&& it : components.stagesM->getStageCallables()) {

				stagesMTPool.create_thread([callable = it.second]() {

					while (true) {

						boost::this_thread::interruption_point();

						callable();
					}
				});
			}

			for (auto&& it : components.monitoringM->getCallables()) {

				monitoringMTPool.create_thread([it]() {

					while (true) {

						boost::this_thread::interruption_point();

						it();
					}
				});
			}

			for (auto&& it : components.sNetworkM) {

				sNetworkMTPool.create_thread([it]() {

					while (true) {

						boost::this_thread::interruption_point();

						(*it)();
					}
					});
			}

			for (auto&& it : components.scalableM->getThreadCallables()) {

				scalableMTPool.create_thread([it]() {

					while (true) {

						boost::this_thread::interruption_point();

						it();
					}
					});
			}
		}

		void stop() {

			rNetworkMTPool.interrupt_all();
			stagesMTPool.interrupt_all();
			monitoringMTPool.interrupt_all();
			sNetworkMTPool.interrupt_all();
			scalableMTPool.interrupt_all();

			rNetworkMTPool.join_all();
			stagesMTPool.join_all();

			components.monitoringM->getStopFunction()();
			monitoringMTPool.join_all();

			sNetworkMTPool.join_all();

			scalableMTPool.join_all();
		}
	};
}