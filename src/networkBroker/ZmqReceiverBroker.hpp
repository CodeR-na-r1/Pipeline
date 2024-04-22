#pragma once

#include "IBrokerReceiver.hpp"

#include <string>
#include <memory>
#include <thread>
#include <chrono>

#include "zmq.hpp"

namespace Pipeline {

    namespace Broker {

        using namespace std::chrono_literals;

        class ZmqReceiverBroker : public IBrokerReceiver<std::shared_ptr<zmq::message_t>> {

        public:

            using BrokerInputT = std::shared_ptr<zmq::message_t>;

        private:

            std::string ip{};
            const int port{};

            zmq::context_t ctx{};
            std::unique_ptr<zmq::socket_t> sck{};

            BrokerInputT msg{};

        public:

            ZmqReceiverBroker() = delete;

            ZmqReceiverBroker(const std::string ip, const int port) : IBrokerReceiver<BrokerInputT>(), ip(ip), port(port) {}

            ZmqReceiverBroker(const ZmqReceiverBroker&) = delete;
            ZmqReceiverBroker(ZmqReceiverBroker&&) = default;

            std::unique_ptr<IBrokerReceiver> build() {

                return std::unique_ptr<IBrokerReceiver>{ new ZmqReceiverBroker{ std::move(*this) } };
            }

            virtual void connect() override {

                sck.reset(new zmq::socket_t{ ctx, zmq::socket_type::sub });
                sck->connect(std::string{ "tcp://" + ip + ":" + std::to_string(port) });
                sck->set(zmq::sockopt::subscribe, "");
            }

            virtual BrokerInputT handleMessage() override {

                if (*sck) { // connection check

                    msg.reset(new zmq::message_t);

                    zmq::recv_result_t result = sck->recv(*msg, zmq::recv_flags::dontwait);

                    if (!result) {
                        std::this_thread::sleep_for(10ms);
                        return {};
                    }

                    return msg;
                }

                throw std::runtime_error{ "Broker not connected! Call \'connect\' method! <from ZmqReceiverBroker::handleMessage()>" };
            }
        };
    }
}
