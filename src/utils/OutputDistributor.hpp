#pragma once

#include "json/json.hpp"

#include <fstream>
#include <stdexcept>

#include <string>
#include <vector>

#include "../builder/SendlerNetworkManagerBuilder.hpp"

#include "../networkBroker/ZmqSendlerBroker.hpp"
#include "../protoManager/SerializationManager.hpp"

namespace Pipeline {

	namespace Utils {

		using nlohmann::json;

		template <typename Traits>
		decltype(auto) outputDistributor(auto& pipeBuilder, const std::string& configFileName) {

			std::ifstream configFile{ configFileName };
			if (!configFile.is_open()) {
				throw std::runtime_error{ "File with configuration not found! <from function \'outputDistributor\'>" };
			}
			json data = json::parse(configFile);
			configFile.close();

			if (data.contains("out")) {

				auto&& outputs = data["out"];

				for (auto&& output : outputs) {

					if (output.contains("afterStage") && output.contains("ip") && output.contains("port")) {

						auto&& stageName = output["afterStage"].get<std::string>();
						auto&& ip = output["ip"].get<std::string>();
						auto&& port = std::atoi(output["port"].get<std::string>().c_str());

						pipeBuilder.addSendlerNetworkManager(
							Builder::SendlerNetworkManagerBuilder<Traits>{}
								.addNameStageForConnectManager(stageName)
								.setBroker(Broker::ZmqSendlerBroker{ ip, port }.build())
								.setSerializator(Proto::NDArraySerializator<typename Traits::DataT>{}.build())
							.build()
						);
					}
					else {
						throw std::runtime_error{ "Error configuration parse in \'outputDistributor\'. Fields \'afterStage\', \'Ip\' or \'port\' not set for output!" };
					}
				}
			}

			return pipeBuilder;
		}
	}
}
