#pragma once

#include <memory>
#include <functional>
#include <stdexcept>

#include "IReceiverNetworkManagerBuilder.hpp"

#include "../NetworkManager/StandartReceiverNetworkManager.hpp"

#include "../NetworkBroker/IBrokerReceiver.hpp"
#include "../ProtoManager/IDeserializationManager.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename Traits>
		class ReceiverNetworkManagerBuilder : public IReceiverNetworkManagerBuilder<typename Traits::DataT, typename Traits::DaoT> {

			using DataT = typename Traits::DataT;
			using BrokerInputT = typename Traits::BrokerInputT;
			using DaoT = typename Traits::DaoT;

			std::unique_ptr<Broker::IBrokerReceiver<BrokerInputT>> broker{};
			std::unique_ptr<Proto::IDeserializationManager<BrokerInputT, DataT>> deserializator{};

		public:

			ReceiverNetworkManagerBuilder() : IReceiverNetworkManagerBuilder<DataT, DaoT>() {}

			ReceiverNetworkManagerBuilder(ReceiverNetworkManagerBuilder&&) = default;

			ReceiverNetworkManagerBuilder& setBroker(std::unique_ptr<Broker::IBrokerReceiver<BrokerInputT>> iBroker) {

				broker = std::move(iBroker);

				return *this;
			}

			ReceiverNetworkManagerBuilder& setDeserializator(std::unique_ptr<Proto::IDeserializationManager<BrokerInputT, DataT>> iDeserializator) {

				deserializator = std::move(iDeserializator);

				return *this;
			}

			[[nodiscard]]
			std::unique_ptr<ReceiverNetworkManagerBuilder> build() {

				if (!deserializator || !broker) {
					throw std::runtime_error{ "Not all fields are initialized, use \'build\' methods!" };
				}

				return std::unique_ptr<ReceiverNetworkManagerBuilder> {new ReceiverNetworkManagerBuilder{ std::move(*this) }};
			}

			[[nodiscard]]
			virtual std::shared_ptr<Network::IReceiverNetworkManager> build(std::shared_ptr<Connector::IConnector<DaoT>> queue, std::function<DaoT(DataT)> mapper) override {

				return std::shared_ptr<Network::IReceiverNetworkManager>{ new Network::StandartReceiverNetworkManager<BrokerInputT, DataT, DaoT>{
					std::move(broker),
					std::move(deserializator),
					queue,
					mapper }
				};
			}
		};
	}
}