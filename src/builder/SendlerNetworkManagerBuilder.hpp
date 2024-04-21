#pragma once

#include <stdexcept>

#include "ISendlerNetworkManagerBuilder.hpp"

#include "SendlerNetworkManagerAssembly.hpp"
#include "../networkManager/StandartSendlerNetworkManager.hpp"

#include "../networkBroker/IBrokerSendler.hpp"
#include "../protoManager/ISerializationManager.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename Traits>
		class SendlerNetworkManagerBuilder : public ISendlerNetworkManagerBuilder<typename Traits::DataT, typename Traits::DaoT> {

			using DataT = typename Traits::DataT;
			using BrokerOutputT = typename Traits::BrokerOutputT;
			using DaoT = typename Traits::DaoT;

			std::string nameStage{};
			std::shared_ptr<Broker::IBrokerSendler<BrokerOutputT>> broker{};
			std::shared_ptr<Proto::ISerializationManager<DataT, BrokerOutputT>> serializator{};

		public:

			SendlerNetworkManagerBuilder() : ISendlerNetworkManagerBuilder<DataT, DaoT>() {}

			SendlerNetworkManagerBuilder(SendlerNetworkManagerBuilder&&) = default;

			SendlerNetworkManagerBuilder& addNameStageForConnectManager(const std::string& nameStage) {

				this->nameStage = nameStage;

				return *this;
			}

			SendlerNetworkManagerBuilder& setBroker(std::shared_ptr<Broker::IBrokerSendler<BrokerOutputT>> iBroker) {

				broker = iBroker;

				return *this;
			}

			SendlerNetworkManagerBuilder& setSerializator(std::shared_ptr<Proto::ISerializationManager<DataT, BrokerOutputT>> iSerializator) {

				serializator = iSerializator;

				return *this;
			}

			[[nodiscard]]
			std::unique_ptr<ISendlerNetworkManagerBuilder<DataT, DaoT>> build() {

				if (!serializator || !broker) {
					throw std::runtime_error{ "Not all fields are initialized, use \'build\' methods!" };
				}

				return std::unique_ptr<ISendlerNetworkManagerBuilder<DataT, DaoT>> {new SendlerNetworkManagerBuilder{ std::move(*this) }};
			}

			virtual const std::string& getStageConnectName() override {

				return nameStage;
			}

			[[nodiscard]]
			virtual std::shared_ptr<Network::ISendlerNetworkManager> build(std::shared_ptr<Connector::IConnector<DaoT>> queue, std::function<DataT(DaoT)> mapper) override {

				SendlerNetworkManagerAssembly<BrokerOutputT, DataT, DaoT> sAsm{ serializator, broker };

				sAsm.init(queue, mapper);

				return std::shared_ptr<Network::ISendlerNetworkManager>{ new Network::StandartSendlerNetworkManager{ sAsm.callable }};
			}
		};
	}
}