#include <iostream>

#include <memory>

#include <vector>
#include <unordered_map>

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

#include "parser/parser.hpp"
#include "pipeline/stage.hpp"

using namespace std;
using TensorT = boost::numeric::ublas::tensor<double>;
using ShapeT = boost::numeric::ublas::shape;

Pipeline::Stage<TensorT> getStages() {

    ifstream fileJsonConfig("../pipeConfig.json");

    if (!fileJsonConfig.is_open()) {
        cerr << "File from object ifstream \'fileJsonConfig\' not open!" << endl;
        throw std::exception{ "FILE CONFIG NOT FOUND" };
    }

    Pipeline::Stage<TensorT> source = Pipeline::JsonParser::fromFile<TensorT>(fileJsonConfig, [](std::string callableName) -> function<TensorT(TensorT)> {

        if (callableName == "getImage") {
            return [](TensorT data) { return data; };
        }
        else if (callableName == "rgb2gray") {
            return [](TensorT data) { return data; };
        }
        else if (callableName == "gaussian") {
            return [](TensorT data) { return data; };
        }
        else if (callableName == "display") {
            return [](TensorT data) { cout << data[0]; return data; };
        }

        return [](TensorT data) { return data; };
        }).value_or(Pipeline::Stage<TensorT>{ [](TensorT data) {return data; }, {}, {} });

        return source;
}

int main() {

    {
        // InputNetworkManager {begin}
        // 
        // prepare getting data from network via zmq

        const char* ip = "127.0.0.1";
        const int port = 5555;

        zmq::context_t ctx;

        boost::lockfree::spsc_queue<std::shared_ptr<TensorT>, boost::lockfree::capacity<1024> > rawDataQ;  // buffer

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
                    TensorT* tensor = new TensorT{ shapeTensor };

                    auto data = ndarray.getData().begin();
                    for (size_t i(0ul); i < tensor->extents().product(); ++i)
                    {
                        (*tensor)[i] = *(data++);  // I have not been able to use std::copy here :(
                    }
                    
                    std::shared_ptr<TensorT> temp{ tensor };
                    while (!rawDataQ.push(temp)) {}

                    msg.rebuild();  // CHECK THIS
                }
            }
        );

        cout << "size queue -> " << rawDataQ.read_available() << endl;

        cout << "Wait while input queue will filled" << endl;
        //Sleep(10000);    // work emulation

        cout << "size queue -> " << rawDataQ.read_available() << endl;

        // InputNetworkManager {end}
        // 
        // Pipeline -> 'ProcessingManager' {begin}

        // Creating queue for each stage
        Pipeline::Stage<TensorT> stages = getStages();
        cout << stages << endl;
        cout << "ID -> " << stages.childs[1].id << endl;

        // create map for stages launch in multi-threading

        std::unordered_map<size_t, Pipeline::Stage<TensorT>> stagesMap; // id ->  stage by id
        std::unordered_map<size_t, std::shared_ptr<boost::lockfree::spsc_queue<std::shared_ptr<TensorT>>>> inputQueueMap; // get parent queue for current stage by id (i.e. get input queue)
        std::unordered_map<size_t, std::vector<std::shared_ptr<boost::lockfree::spsc_queue<std::shared_ptr<TensorT>>>>> outputQueueMap; // get parent queue for current stage by id (i.e. get input queue)

        // fill 1 map
        std::queue<Pipeline::Stage<TensorT>*> queue;
        queue.push(&stages);

        while (!queue.empty()) {

            auto current = queue.front();

            outputQueueMap.insert({ current->id, {} });
            outputQueueMap.at(current->id).reserve(current->childs.size());

            for (auto&& child: current->childs) {

                std::shared_ptr<boost::lockfree::spsc_queue<std::shared_ptr<TensorT>>> temp = { {} };
                outputQueueMap.at(current->id).push_back(temp);
                inputQueueMap.insert({ child.id , temp });
                queue.push(&child);
            }

            stagesMap.insert({ current->id, (*current) });

            queue.pop();
        }

        // fill 2 and 3 map's


        

        // Pipeline -> 'ProcessingManager' {end}
        //
        //---------
        // 
        // Part of InputNetworkManager (stopping threadpool)
        networkThreadpool.interrupt_all();  /// interrupts threads
        networkThreadpool.join_all();   // join threads
    }

    return 0;
}