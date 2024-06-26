#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include "memory"

#include "../../parser/DLParser.hpp"

#include "DL.hpp"
#include "../../dLManager/DLManager.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT>
		class DLBuilder {

			std::string configFileName{};

			std::vector<std::pair<std::string, std::function<DataT(DataT)>>> chooser;

		public:

			DLBuilder& addConfigFileName(const std::string& configFileName) {

				this->configFileName = configFileName;

				return *this;
			}

			std::shared_ptr<DL::IDLManager> build() {

				std::ifstream configFile{ configFileName };

				if (!configFile.is_open())
					throw std::runtime_error{ "File with configuration not open! <from Dynamic Library Builder (ScalableParallelPipelineBuilder)>" };

				auto&& dlProperties = Parser::DLParser::getDLProperties(configFile);
				configFile.close();

				std::vector<std::shared_ptr<DL::DL>> DLs{};

				for (auto&& dlFileName : dlProperties) {

					std::filesystem::path p{ dlFileName };
					p.make_preferred();

					std::shared_ptr<DL::DL> dl;
					dl.reset(new DL::DL{ p.string() });

					if (!(*dl)(reinterpret_cast<void*>(&chooser)))
						throw std::runtime_error{ std::string{"Error with "} + dlFileName + std::string{" DL! <from Dynamic Library Builder (ScalableParallelPipelineBuilder)>"} };

					DLs.push_back(dl);
				}

				return std::shared_ptr<DL::IDLManager>{new DL::DLManager{ DLs }};
			}

			const std::vector<std::pair<std::string, std::function<DataT(DataT)>>>& getChoosers() {

				return chooser;
			}
		};
	}
}