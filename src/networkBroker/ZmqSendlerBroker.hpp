#pragma once

#include "IBrokerSendler.hpp"

#include <string>
#include <memory>
#include <chrono>

#include "zmq.hpp"

namespace Pipeline {

    namespace Broker {

        using namespace std::chrono_literals;

        class ZmqSendlerBroker : public IBrokerSendler<zmq::message_t> {

        public:

            using BrokerOutputT = zmq::message_t;

        private:

            std::string ip{};
            const int port{};

            zmq::context_t ctx{};
            std::unique_ptr<zmq::socket_t> sck{};

        public:

            ZmqSendlerBroker() = delete;

            ZmqSendlerBroker(const std::string ip, const int port) : IBrokerSendler<BrokerOutputT>(), ip(ip), port(port) {}

            ZmqSendlerBroker(ZmqSendlerBroker&&) = default;

            std::shared_ptr<IBrokerSendler> build() {

                return std::shared_ptr<IBrokerSendler>{ new ZmqSendlerBroker{ std::move(*this) } };
            }

            virtual void connect() override {

                sck.reset(new zmq::socket_t{ ctx, zmq::socket_type::pub });
                sck->bind(std::string{ "tcp://" + ip + ":" + std::to_string(port) });
            }

            virtual bool handleMessage(BrokerOutputT msg) override {

                if (*sck) { // connection check

                    sck->send(msg);

                    return true;
                }

                throw std::runtime_error{ "Broker not connected! Call \'connect\' method! <from ZmqReceiverBroker::handleMessage()>" };
            }
        };
    }
}