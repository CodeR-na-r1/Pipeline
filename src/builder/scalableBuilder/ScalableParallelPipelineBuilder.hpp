#pragma once

#include <memory>
#include <functional>

#include <vector>

#include "../../pipeline/frame/IDFrame.hpp"
#include "../../detail/pipeline/ScalableParallelPipelineComponents.hpp"
#include "../../pipeline/ScalableParallelPipeline.hpp"

#include "../IReceiverNetworkManagerBuilder.hpp"
#include "../ISendlerNetworkManagerBuilder.hpp"
#include "IScalableStageBuilder.hpp"
#include "../IMonitoringBuilder.hpp"
#include "IScalableManagerBuilder.hpp"
#include "../../detail/dl/DLBuilder.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT>
		class ScalableParallelPipelineBuilder {

		public:

			using DaoT = Frame::IDFrame<std::size_t, DataT>;

		private:

			std::vector<std::unique_ptr<IReceiverNetworkManagerBuilder<DataT, DaoT>>> inputNMBuilder{};
			std::vector<std::unique_ptr<ISendlerNetworkManagerBuilder<DataT, DaoT>>> outputNMBuilder{};
			std::unique_ptr<IScalableStageBuilder<DataT, DaoT>> stageBuilder{};
			std::unique_ptr<IMonitoringBuilder<DaoT>> monitoringBuilder{};
			std::unique_ptr<IScalableManagerBuilder> scalableBuilder{};

			std::function<DaoT(DataT)> mapperToNewDao = [](DataT data) { static std::size_t idCounter{}; return DaoT{ idCounter++, data }; };
			std::function<DaoT(DaoT, DataT)> mapperToDao = [](DaoT oldDao, DataT data) { return DaoT{ oldDao.getID(), data }; };
			std::function<DataT(DaoT)> mapperfromDao = [](DaoT data) { return data.getData(); };

		public:

			ScalableParallelPipelineBuilder() = default;

			ScalableParallelPipelineBuilder& addReceiverNetworkManager(std::unique_ptr<IReceiverNetworkManagerBuilder<DataT, DaoT>> iNMBuilder) {

				inputNMBuilder.emplace_back(std::move(iNMBuilder));

				return *this;
			}

			ScalableParallelPipelineBuilder& addSendlerNetworkManager(std::unique_ptr<ISendlerNetworkManagerBuilder<DataT, DaoT>> iNMBuilder) {

				outputNMBuilder.emplace_back(std::move(iNMBuilder));

				return *this;
			}

			ScalableParallelPipelineBuilder& setStages(std::unique_ptr<IScalableStageBuilder<DataT, DaoT>> iSBuilder) {

				stageBuilder = std::move(iSBuilder);

				return *this;
			}

			ScalableParallelPipelineBuilder& addMonitoringManager(std::unique_ptr<IMonitoringBuilder<DaoT>> imBuilder) {

				monitoringBuilder = std::move(imBuilder);

				return *this;
			}

			ScalableParallelPipelineBuilder& addScalableManager(std::unique_ptr<IScalableManagerBuilder> isBuilder) {

				scalableBuilder = std::move(isBuilder);

				return *this;
			}

			[[nodiscard]]
			ScalableParallelPipeline<DataT, DaoT> build() {

				auto&& dlBuilder = DLBuilder<DataT>{};
				dlBuilder.addConfigFileName(stageBuilder->getConfigFileName());

				auto&& dlManager = dlBuilder.build();

				for (auto&& it : dlBuilder.getChoosers())
					stageBuilder->addChooser(it);

				// get queues
				std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>> outputStages{}; outputStages.reserve(outputNMBuilder.size());
				for (auto&& it : outputNMBuilder) {

					outputStages.emplace_back(it->getStageConnectName(), std::shared_ptr<Connector::IConnector<DaoT>>{});
				}

				auto&& stageManager = stageBuilder->build(inputNMBuilder.size() == 1, mapperfromDao, mapperToDao, outputStages);

				auto&& receiveConnector = stageManager->getinputQMap().at(stageBuilder->getStages().getId());

				std::vector<std::shared_ptr<Network::IReceiverNetworkManager>> vRNManager{};
				for (auto&& itB : inputNMBuilder) {
					
					vRNManager.push_back(itB->build(receiveConnector, mapperToNewDao));
				}

				std::vector<std::shared_ptr<Network::ISendlerNetworkManager>> vSNManager{};
				for (std::size_t i{}; i < outputNMBuilder.size(); ++i) {

					vSNManager.push_back(outputNMBuilder[i]->build(outputStages[i].second, mapperfromDao));
				}

				auto&& scalableManager = scalableBuilder->build(stageBuilder->getScalableProperties(), stageManager->getStageCallables());

				monitoringBuilder->addMonitoringCallback(scalableManager->getMonitoringCallback());
				auto&& monitoringManager = monitoringBuilder->build(stageManager->getinputQMap(), stageManager->getMeasurementMap());

				stageBuilder->resetStages();

				return Pipeline::ScalableParallelPipeline<DataT, DaoT>{ detail::ScalableParallelPipelineComponents<DataT, DaoT>{ .dlM = dlManager, .rNetworkM = vRNManager, .stagesM = stageManager, .monitoringM = monitoringManager, .sNetworkM = vSNManager, .scalableM = scalableManager } };
			}
		};
	}
}