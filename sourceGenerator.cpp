#include <iostream>

#include <vector>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "scheme/ndarray.capnp.h"
#include "capnp/serialize.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

#include "boost/asio.hpp"
#include "boost/asio/thread_pool.hpp"
#include <boost/thread/thread.hpp>

#include "boost/lockfree/spsc_queue.hpp"

#include "boost/numeric/ublas/tensor/tensor.hpp"
#include <boost/numeric/ublas/tensor/extents.hpp>   // extents == shape

using namespace std;
using TensorT = boost::numeric::ublas::tensor<double>;
using ShapeT = boost::numeric::ublas::shape;

int main() {

    {
        // prepare getting data from network via zmq

        const char* ip = "127.0.0.1";
        const int port = 5555;

        zmq::context_t ctx;

        boost::lockfree::spsc_queue<TensorT, boost::lockfree::capacity<1024> > rawDataQ;  // buffer

        //boost::asio::io_service ioService;  // for threads (::run)
        boost::thread_group networkThreadpool;
        networkThreadpool.create_thread([&ctx, &rawDataQ]()
            {
                cout << "threadstart" << endl;
                zmq::socket_t dataReceiver(ctx, zmq::socket_type::sub);
                dataReceiver.connect("tcp://127.0.0.1:5555");
                dataReceiver.set(zmq::sockopt::subscribe, "");

                zmq::message_t msg;
                std::vector<std::size_t> shapeVector{}; shapeVector.reserve(3);

                while (true) {

                    cout << "request" << endl;

                    boost::this_thread::interruption_point();

                    zmq::recv_result_t result = dataReceiver.recv(msg, zmq::recv_flags::dontwait);  // change flag after debugging

                    if (!result)  // instead of assert below
                    {
                        Sleep(1000);    // delete after debugging (and change flag)
                        continue;
                    }
                    //assert(result && "recv failed");

                    std::cout << msg.to_string() << endl;

                    // create a memory buffer from the received message
                    auto buf = kj::heapArray<capnp::word>(msg.size() / sizeof(capnp::word));
                    memcpy(buf.asBytes().begin(), msg.data(), buf.asBytes().size());

                    // create an input stream from the memory buffer
                    capnp::FlatArrayMessageReader message_reader(buf);
                    NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

                    // deserialization and putting data into a tensor
                    // 
                    // shape
                    if (ndarray.getShape().size() == 1)
                        shapeVector.push_back(1);

                    for (auto&& dimm : ndarray.getShape()) {
                        shapeVector.push_back(static_cast<std::size_t>(dimm));
                    }
                    ShapeT shapeTensor{ {shapeVector.crbegin(), shapeVector.crend()} };
                    shapeVector.clear();

                    //
                    // data
                    TensorT tensor{ shapeTensor };

                    auto data = ndarray.getData().begin();
                    for (size_t i(0ul); i < tensor.extents().product(); ++i)
                    {
                        tensor[i] = *(data++);  // I have not been able to use std::copy here :(
                    }
                    
                    while (!rawDataQ.push(tensor)) {}

                    msg.rebuild();  // CHECK THIS
                }
            }
        );

        cout << "size queue -> " << rawDataQ.read_available() << endl;

        Sleep(10000);    // work emulation
        networkThreadpool.interrupt_all();  /// interrupts threads
        networkThreadpool.join_all();   // join threads

        cout << "size queue -> " << rawDataQ.read_available() << endl;
    }

    return 0;
}