#pragma once

#include <string>
#include <memory>

#include "zmq.hpp"

namespace Pipeline {

    class INetworkReceiveManager {

    public:

        virtual void connect() = 0;

        virtual bool handleMessage() = 0;
    };

    template <typename Traits>
        requires requires (Traits traits, typename Traits::DeserializerT deserializator) {

        typename Traits::DataT;
        typename Traits::TensorT;
        typename Traits::QueueT;
        typename Traits::DeserializerT;

        deserializator(std::shared_ptr<zmq::message_t>{});

    }
    class ZmqReceiveManager :public INetworkReceiveManager {

        std::string ip;
        const int port;

        zmq::context_t ctx;
        std::unique_ptr<zmq::socket_t> sck;

        std::shared_ptr<zmq::message_t> msg;

        std::shared_ptr<typename Traits::QueueT> queue;
        typename Traits::DeserializerT deserializator;

    public:

        ZmqReceiveManager() = delete;

        ZmqReceiveManager(const std::string ip, const int port, std::shared_ptr<typename Traits::QueueT> queue) :ip(ip), port(port), queue(queue) {}

        virtual void connect() override {

            sck.reset(new zmq::socket_t{ ctx, zmq::socket_type::sub });
            sck->connect(std::string{ "tcp://" + ip + ":" + to_string(port) });
            sck->set(zmq::sockopt::subscribe, "");
        }

        virtual bool handleMessage() override {

            if (*sck) { // connection check

                msg.reset(new zmq::message_t);

                zmq::recv_result_t result = sck->recv(*msg, zmq::recv_flags::dontwait);  // change flag after debugging

                if (!result)  // instead of assert below
                {
                    std::this_thread::sleep_for(10ms);    // delete after debugging (and change flag)
                    return false;
                }

                auto tensor = deserializator(msg);

                while (!queue->push(tensor)) {}

                return true;
            }

            return false;
        }
    };
}