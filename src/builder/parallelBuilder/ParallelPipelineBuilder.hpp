#pragma once

#include <memory>
#include <functional>

#include <vector>

#include "../../pipeline/frame/Frame.hpp"
#include "../../detail/pipeline/ParallelPipelineComponents.hpp"
#include "../../pipeline/ParallelPipeline.hpp"

#include "../IReceiverNetworkManagerBuilder.hpp"
#include "../ISendlerNetworkManagerBuilder.hpp"
#include "IStageBuilder.hpp"
#include "../IMonitoringBuilder.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT>
		class ParallelPipelineBuilder {

		public:

			using DaoT = Frame::Frame<DataT>;

		private:

			std::vector<std::unique_ptr<IReceiverNetworkManagerBuilder<DataT, DaoT>>> inputNMBuilder{};
			std::vector<std::unique_ptr<ISendlerNetworkManagerBuilder<DataT, DaoT>>> outputNMBuilder{};
			std::unique_ptr<IStageBuilder<DataT, DaoT>> stageBuilder{};
			std::unique_ptr<IMonitoringBuilder<DaoT>> monitoringBuilder{};

			std::function<DaoT(DataT)> mapperToNewDao = [](DataT data) { return Frame::Frame{ data }; };
			std::function<DaoT(DaoT, DataT)> mapperToDao = [](DaoT oldDao, DataT data) { return Frame::Frame{ data }; };
			std::function<DataT(DaoT)> mapperfromDao = [](DaoT data) { return data.getData(); };

		public:

			ParallelPipelineBuilder() = default;

			ParallelPipelineBuilder& addReceiverNetworkManager(std::unique_ptr<IReceiverNetworkManagerBuilder<DataT, DaoT>> iNMBuilder) {

				inputNMBuilder.emplace_back(std::move(iNMBuilder));

				return *this;
			}

			ParallelPipelineBuilder& addSendlerNetworkManager(std::unique_ptr<ISendlerNetworkManagerBuilder<DataT, DaoT>> iNMBuilder) {

				outputNMBuilder.emplace_back(std::move(iNMBuilder));

				return *this;
			}

			ParallelPipelineBuilder& setStages(std::unique_ptr<IStageBuilder<DataT, DaoT>> iSBuilder) {

				stageBuilder = std::move(iSBuilder);

				return *this;
			}

			ParallelPipelineBuilder& addMonitoringManager(std::unique_ptr<IMonitoringBuilder<DaoT>> imBuilder) {

				monitoringBuilder = std::move(imBuilder);

				return *this;
			}

			[[nodiscard]]
			ParallelPipeline<DataT, DaoT> build() {

				// get queues
				std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>> outputStages{}; outputStages.reserve(outputNMBuilder.size());
				for (auto&& it : outputNMBuilder) {

					outputStages.emplace_back(it->getStageConnectName(), std::shared_ptr<Connector::IConnector<DaoT>>{});
				}

				auto&& stageManager = stageBuilder->build(outputStages, mapperfromDao, mapperToDao);

				auto&& receiveConnector = stageManager->getinputQMap().at(stageBuilder->getStages().getId());

				std::vector<std::shared_ptr<Network::IReceiverNetworkManager>> vRNManager{};
				for (auto&& itB : inputNMBuilder) {
					
					vRNManager.push_back(itB->build(receiveConnector, mapperToNewDao));
				}

				std::vector<std::shared_ptr<Network::ISendlerNetworkManager>> vSNManager{};
				for (std::size_t i{}; i < outputNMBuilder.size(); ++i) {

					vSNManager.push_back(outputNMBuilder[i]->build(outputStages[i].second, mapperfromDao));
				}

				auto&& monitoringManager = monitoringBuilder->build(stageManager->getinputQMap(), stageManager->getMeasurementMap());

				return Pipeline::ParallelPipeline<DataT, DaoT>{ detail::ParallelPipelineComponents<DataT, DaoT>{ .rNetworkM = vRNManager, .stagesM = stageManager, .monitoringM = monitoringManager, .sNetworkM = vSNManager } };
			}
		};
	}
}