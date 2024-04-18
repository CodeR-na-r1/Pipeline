#pragma once

#include <memory>
#include <string>
#include <vector>
#include "unordered_set"
#include <iterator>
#include <concepts>
#include <functional>
#include <stdexcept>

#include "IScalableStageBuilder.hpp"
#include "../../detail/chooser/Chooser.hpp"
#include "../../Parser/Parser.hpp"
#include "../../Parser/ScalablePropertyParser.hpp"
#include "../../Parser/SynchronyzePropertyParser.hpp"
#include "../../StageManager/StageManager.hpp"

#include "ScalableStageAssembly.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename Traits>
		class ScalableStageBuilder : public IScalableStageBuilder<typename Traits::DataT, typename Traits::DaoT> {

			using DataT = typename Traits::DataT;
			using DaoT = typename Traits::DaoT;

			std::string configFilename{};
			detail::Chooser<DataT> _chooser{};

			Stage::Stage<DataT> stages{};
			std::unordered_set<std::size_t> scalableProperties{};
			std::unordered_set<std::size_t> synchronyzeProperties{};

		public:

			ScalableStageBuilder() : IScalableStageBuilder<DataT, DaoT>() {}

			ScalableStageBuilder& addConfigFile(const std::string& configFilename) {

				this->configFilename = configFilename;

				return *this;
			}

			virtual ScalableStageBuilder& addChooser(const std::pair<std::string, std::function<DataT(DataT)>>& chooser) override {

				_chooser.addChoser(chooser);

				return *this;
			}

			template <typename ItType>	// ADD CONCEPT <std::input_iterator ItF, std::input_iterator ItL>
			ScalableStageBuilder& addChoosers(ItType first, ItType last) {

				while (first != last) {

					_chooser.addChoser(*first);
					++first;
				}

				return *this;
			}

			[[nodiscard]]
			std::unique_ptr<IScalableStageBuilder<DataT, DaoT>> build() {

				return std::unique_ptr<IScalableStageBuilder<DataT, DaoT>>{ new ScalableStageBuilder<Traits>{ std::move(*this) }};
			}

			[[nodiscard]]
			virtual std::shared_ptr<Stage::IStageManager<DataT, DaoT>> build(bool isOneRNManager, std::function<DataT(DaoT)> mapperFromDao, std::function<DaoT(DaoT, DataT)> mapperToDao, std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>>& outputQueues) override {

				parseConfig();

				ScalableStageAssembly<DataT, DaoT> sAsm{};

				sAsm.initStages(stages, scalableProperties, synchronyzeProperties, isOneRNManager, outputQueues);

				sAsm.initCallables(mapperFromDao, mapperToDao);

				return std::shared_ptr<Stage::IStageManager<DataT, DaoT>>{ new Stage::StageManager<DataT, DaoT>{ std::move(sAsm.stagesMap), sAsm.inputQueueMap, sAsm.outputQueuesMap, sAsm.measurementsMap, sAsm.stageCallables }};
			}

			virtual const Stage::Stage<DataT>& getStages() override {

				return stages;
			}

			virtual std::unordered_set<std::size_t> getScalableProperties() override {

				return scalableProperties;
			}

			virtual const std::string& getConfigFileName() override {

				return configFilename;
			}

			private:

			void parseConfig() {

				// open config file
				std::ifstream configFile{ configFilename };

				if (!configFile.is_open())
					throw std::runtime_error{ "Config file not open! <from Stagebuilder>" };

				// parse stages
				auto&& resParser = Parser::JsonParser::fromFile<DataT>(configFile, std::shared_ptr<detail::IChooser<DataT>>(new detail::Chooser<DataT>{ std::move(_chooser) }));

				if (!resParser.has_value())
					throw std::runtime_error{ "Configuration error! <from parser>" };

				stages = std::move(resParser.value());

				// reset state of file
				configFile.clear();
				configFile.seekg(0, std::ios::beg);

				// parse scalable properties
				auto&& scalableResParser = Parser::ScalablePropertiesParser::getScalableProperties(configFile, stages);

				if (!scalableResParser.has_value()) {
					throw std::runtime_error{ "Configuration error! <from ScalablePropertiesParser>" };
				}

				scalableProperties = scalableResParser.value();

				// reset state of file
				configFile.clear();
				configFile.seekg(0, std::ios::beg);

				// parse synchronyze properties
				auto&& synchronyzeResParser = Parser::SynchronyzePropertiesParser::getSynchronyzeProperties(configFile, stages);

				if (!synchronyzeResParser.has_value()) {
					throw std::runtime_error{ "Configuration error! <from SynchronyzePropertiesParser>" };
				}

				synchronyzeProperties = synchronyzeResParser.value();

				configFile.close();
			}
		};
	}
}