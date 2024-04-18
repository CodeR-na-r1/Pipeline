#pragma once

#include "IReceiverNetworkManager.hpp"

#include <memory>

#include "../NetworkBroker/IBrokerReceiver.hpp"
#include "../ProtoManager/IDeserializationManager.hpp"
#include "../connector/IConnector.hpp"

namespace Pipeline {

	namespace Network {

		template <typename BrokerInputT, typename DataT, typename DaoT>
		class StandartReceiverNetworkManager : public IReceiverNetworkManager {

			std::unique_ptr<Broker::IBrokerReceiver<BrokerInputT>> broker;
			std::unique_ptr<Proto::IDeserializationManager<BrokerInputT, DataT>> deserializator;
			std::shared_ptr<Connector::IConnector<DaoT>> queue;
			std::function<DaoT(DataT)> mapper;

		public:

			StandartReceiverNetworkManager() = delete;

			StandartReceiverNetworkManager(std::unique_ptr<Broker::IBrokerReceiver<BrokerInputT>> broker,
				std::unique_ptr<Proto::IDeserializationManager<BrokerInputT, DataT>> deserializator,
				std::shared_ptr<Connector::IConnector<DaoT>> queue,
				std::function<DaoT(DataT)> mapper)
				: IReceiverNetworkManager(), broker(std::move(broker)), deserializator(std::move(deserializator)), queue(queue), mapper(mapper) {

				this->broker->connect();
			}

			virtual void operator()() override {

				auto&& msg = broker->handleMessage();

				if (msg) {

					auto&& data = (*deserializator)(msg);

					queue->push(mapper(data));
				}
			}
		};
	}
}