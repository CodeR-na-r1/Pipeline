#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iterator>
#include <concepts>
#include <functional>
#include <stdexcept>

#include "IStageBuilder.hpp"
#include "../../detail/chooser/Chooser.hpp"
#include "../../parser/Parser.hpp"
#include "../../stageManager/StageManager.hpp"

#include "StageAssembly.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename Traits>
		class StageBuilder : public IStageBuilder<typename Traits::DataT, typename Traits::DaoT> {

			using DataT = typename Traits::DataT;
			using DaoT = typename Traits::DaoT;

			std::string configFilename{};
			detail::Chooser<DataT> _chooser{};

			Stage::Stage<DataT> stages{};

		public:

			StageBuilder() : IStageBuilder<DataT, DaoT>() {}

			StageBuilder& addConfigFile(const std::string& configFilename) {

				this->configFilename = configFilename;

				return *this;
			}

			StageBuilder& addChooser(const std::pair<std::string, std::function<DataT(DataT)>>& chooser) {

				_chooser.addChoser(chooser);

				return *this;
			}

			template <typename ItType>	// ADD CONCEPT <std::input_iterator ItF, std::input_iterator ItL>
			StageBuilder& addChoosers(ItType first, ItType last) {

				while (first != last) {

					_chooser.addChoser(*first);
					++first;
				}

				return *this;
			}

			[[nodiscard]]
			std::unique_ptr<IStageBuilder<DataT, DaoT>> build() {

				// parse config file
				std::ifstream configFile{ configFilename };

				if (!configFile.is_open())
					throw std::runtime_error{ "Config file not open! <from Stagebuilder>" };

				stages = Parser::JsonParser::fromFile<DataT>(configFile, std::shared_ptr<detail::IChooser<DataT>>(new detail::Chooser<DataT>{ std::move(_chooser) }));

				configFile.close();

				return std::unique_ptr<IStageBuilder<DataT, DaoT>>{ new StageBuilder<Traits>{ std::move(*this) }};
			}

			[[nodiscard]]
			virtual std::shared_ptr<Stage::IStageManager<DataT, DaoT>> build(std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>>& outputQueues, std::function<DataT(DaoT)> mapperFromDao, std::function<DaoT(DaoT, DataT)> mapperToDao) override {

				StageAssembly<Traits> sAsm{};

				sAsm.initStages(stages, outputQueues);

				sAsm.initCallables(mapperFromDao, mapperToDao);

				return std::shared_ptr<Stage::IStageManager<DataT, DaoT>>{ new Stage::StageManager<DataT, DaoT>{ std::move(sAsm.stagesMap), sAsm.inputQueueMap, sAsm.outputQueuesMap, sAsm.measurementsMap, sAsm.stageCallables }};
			}

			virtual const Stage::Stage<DataT>& getStages() override {

				return stages;
			}
		};
	}
}