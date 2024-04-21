#pragma once

#include "json/json.hpp"

#include <fstream>
#include <stdexcept>

#include <string>
#include <vector>

#include "../builder/ReceiverNetworkManagerBuilder.hpp"

#include "../networkBroker/ZmqReceiverBroker.hpp"
#include "../protoManager/DeserializationManager.hpp"

namespace Pipeline {

	namespace Utils {
			
		using nlohmann::json;

		template <typename Traits>
		decltype(auto) inputDistributor(auto& pipeBuilder, const std::string& configFileName) {

			std::ifstream configFile{ configFileName };
			if (!configFile.is_open()) {
				throw std::runtime_error{ "File with configuration not found! <from function \'inputDistributor\'>" };
			}
			json data = json::parse(configFile);
			configFile.close();

			if (data.contains("in")) {

				auto&& inputs = data["in"];

				for (auto&& input : inputs) {

					if (input.contains("ip") && input.contains("port")) {

						auto&& ip = input["ip"].get<std::string>();
						auto&& port = std::atoi(input["port"].get<std::string>().c_str());

						pipeBuilder.addReceiverNetworkManager(
							Builder::ReceiverNetworkManagerBuilder<Traits>{}
								.setBroker(Broker::ZmqReceiverBroker{ ip, port }.build())
								.setDeserializator(Proto::NDArrayDeserializator<typename Traits::DataT>{}.build())
							.build()
							);
					}
					else {
						throw std::runtime_error{ "Error configuration parse in \'inputDistributor\'. Ip or port not set for input!" };
					}
				}
			}

			return pipeBuilder;
		}
	}
}
