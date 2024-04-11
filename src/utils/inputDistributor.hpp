#pragma once

#include "json/json.hpp"

#include <fstream>
#include <stdexcept>

#include <vector>

#include "../builder/ReceiverNetworkManagerBuilder.hpp"

#include "../NetworkBroker/ZmqReceiverBroker.hpp"
#include "../ProtoManager/DeserializationManager.hpp"

namespace Pipeline {

	namespace Utils {

		using nlohmann::json;

		template <typename pipeBuilderT, typename Traits>
		pipeBuilderT inputDistributor(pipeBuilderT&& pipeBuilder, std::ifstream& configFile) {

			json data = json::parse(configFile);

			if (data.contains("in")) {

				auto&& inputs = data["in"];

				for (auto&& input : inputs) {

					if (input.contains("ip") && input.contains("port")) {

						pipeBuilder.addReceiverNetworkManager(
							Builder::ReceiverNetworkManagerBuilder<Traits>{}
							.setBroker(Broker::ZmqReceiverBroker{ input["ip"], input["port"] }.build())
							.setDeserializator(Proto::NDArrayDeserializator<typename Traits::DataT>{}.build())
							.build()
						);
					}
					else {
						throw std::runtime_error{ "Error configuration parse in \'inputDistributor\'. Ip or port not set for input!" };
					}
				}
			}

			return std::forward(pipeBuilder);
		}
	}
}