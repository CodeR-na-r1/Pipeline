#include <iostream>

#include <vector>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "ndarray.capnp.h"
#include "capnp/serialize.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

using namespace std;

namespace Pipe {

    struct MyNDArray {

        std::vector<uint32_t> shape;
        uint64_t timestamp;
    };

}

//struct NDArray {

//};

int main() {

    // 1 sample
    {
        zmq::context_t ctx;
        zmq::socket_t sock(ctx, zmq::socket_type::push);
        sock.bind("inproc://test");
        sock.send(zmq::str_buffer("Hello, world"), zmq::send_flags::dontwait);
    }

    // 2 sample
    {
        const char* ip = "*";
        const int port = 5555;

        zmq::context_t ctx;
        zmq::socket_t dataReceiver(ctx, zmq::socket_type::sub);

        //dataReceiver.connect("tcp://" + *ip + *":" + port);
        dataReceiver.connect("tcp://127.0.0.1:5555");
        dataReceiver.set(zmq::sockopt::subscribe, "");

        //std::vector<zmq::message_t> msgQueue;
        zmq::message_t msg;

        while (true) {
            //1
            zmq::recv_result_t result = dataReceiver.recv(msg, zmq::recv_flags::none);

            assert(result && "recv failed");

            std::cout << msg.to_string() << endl;

            // create a memory buffer from the received message
            //kj::ArrayPtr<capnp::word> buf(reinterpret_cast<capnp::word*>(msg.data()), msg.size() / sizeof(capnp::word)); // lose - because!? (+ ref i on SO)

            auto buf = kj::heapArray<capnp::word>(msg.size() / sizeof(capnp::word));
            memcpy(buf.asBytes().begin(), msg.data(), buf.asBytes().size());

            // create an input stream from the memory buffer
            capnp::FlatArrayMessageReader message_reader(buf);
            NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

            //3 use data from capnp
            Pipe::MyNDArray mnd;

            mnd.timestamp = ndarray.getTimestamp();

            for (auto&& dim : ndarray.getShape()) {
                mnd.shape.push_back(dim);
            }
            
            switch (ndarray.getDtype())
            {
            case NDArray::DType::BOOL:
                break;
            case NDArray::DType::UINT16:
                cout << "UINT16" << endl;
                break;
            case NDArray::DType::FLOAT64:
                cout << "FLOAT64" << endl;
                break;
            default:
                break;
            }

            //2
            //zmq::recv_result_t result = zmq::recv_multipart(dataReceiver, std::back_inserter(msgQueue));

            /*
            for (auto&& msg : msgQueue) {

                std::cout << msg.to_string() << endl;

                //work with capnp here
                // 
                // create a memory buffer from the received message
                kj::ArrayPtr<capnp::word> buffer(reinterpret_cast <capnp::word*>(zmq_message.data()), zmq_message.size() / sizeof(capnp::word));

                // create an input stream from the memory buffer
                capnp::FlatArrayMessageReader message_reader(buffer);
                Message::Reader message = message_reader.getRoot<Message>();
            }
            msgQueue.clear();
            */
        }
    }

	return 0;
}