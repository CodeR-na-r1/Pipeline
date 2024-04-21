#pragma once

#include <memory>
#include <chrono>
#include <functional>

#include "../networkBroker/IBrokerSendler.hpp"
#include "../protoManager/ISerializationManager.hpp"

namespace Pipeline {

    namespace Builder {

        using namespace std::chrono_literals;

        template <typename BrokerT, typename DataT, typename DaoT>
        struct SendlerNetworkManagerAssembly {

            std::shared_ptr<Proto::ISerializationManager<DataT, BrokerT>> serializator{};
            std::shared_ptr<Broker::IBrokerSendler<BrokerT>> broker{};
            std::function<void(void)> callable{};

            SendlerNetworkManagerAssembly() = delete;

            SendlerNetworkManagerAssembly(std::shared_ptr<Proto::ISerializationManager<DataT, BrokerT>> serializator, std::shared_ptr<Broker::IBrokerSendler<BrokerT>> broker) : serializator(serializator), broker(broker) {
                
                broker->connect();
            }

            void init(std::shared_ptr<Connector::IConnector<DaoT>> inputQueue, std::function<DataT(DaoT)> mapper) {
                
                if (callable)
                    return;

                callable = std::function<void(void)>{ [inputQueue, mapper, serializator_ = this->serializator, broker_ = this->broker]() {

                    if (inputQueue->hasValue()) {

                        DaoT argData;
                        if (inputQueue->extract(argData)) {

                            auto&& data = mapper(argData);

                            auto&& serData = (*serializator_)(data);

                            broker_->handleMessage(std::move(serData));
                        }
                    }
                    else {
                        std::this_thread::sleep_for(10ms);
                    }
                } };
            }
        };
    }
}