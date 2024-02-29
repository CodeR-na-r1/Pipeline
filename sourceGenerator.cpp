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

                    cout << "1" << endl;
                    std::cout << msg.to_string() << endl;

                    cout << "2" << endl;
                    // create a memory buffer from the received message
                    auto buf = kj::heapArray<capnp::word>(msg.size() / sizeof(capnp::word));
                    memcpy(buf.asBytes().begin(), msg.data(), buf.asBytes().size());

                    cout << "3" << endl;
                    // create an input stream from the memory buffer
                    capnp::FlatArrayMessageReader message_reader(buf);
                    NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

                    cout << "4" << endl;
                    // deserialization and putting data into a tensor
                    // 
                    // shape
                    if (ndarray.getShape().size() == 1)
                        shapeVector.push_back(1);

                    for (auto&& dimm : ndarray.getShape()) {
                        shapeVector.push_back(static_cast<std::size_t>(dimm));
                    }
                    ShapeT shapeTensor{ shapeVector };
                    shapeVector.clear();
                    cout << "5" << endl;
                    //
                    // data
                    TensorT tensor{ shapeTensor };
                    cout << tensor.size() << endl;
                    //std::copy(ndarray.getData().begin(), ndarray.getData().end(), tensor.data()); // TENSOR HOW FILL ELEMENTS (inner storage in tensor not line?)

                    while (!rawDataQ.push({})) {}

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