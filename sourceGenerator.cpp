#include <iostream>

#include <memory>
#include <chrono>
#include <thread>
#include <stdexcept>

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

#include <xtensor/xarray.hpp>
#include <xtensor/xadapt.hpp>

#include "json/json.hpp"

#include "opencv2/opencv.hpp"

#include "parser/parser.hpp"
#include "pipeline/stage.hpp"

#include "cds/container/vyukov_mpmc_cycle_queue.h"

#include "NetworkManager/NetworkReceptionManager.hpp"

using namespace std;
using namespace chrono_literals;

using json = nlohmann::json;

using DataT = double;
using TensorT = xt::xarray<DataT>;
using ShapeT = xt::svector<size_t>;

using MPMCQueue = cds::container::VyukovMPMCCycleQueue<
    std::shared_ptr<TensorT>, typename cds::container::vyukov_queue::make_traits<
        cds::opt::buffer<
            cds::opt::v::initialized_static_buffer<void*, 1024>>,
        cds::opt::item_counter< cds::atomicity::item_counter>
    >::type>;

using spsc_queueT = boost::lockfree::spsc_queue<std::shared_ptr<TensorT>, boost::lockfree::capacity<1024>>;

using queueT = MPMCQueue;

Pipeline::Stage<TensorT> getStages() {

    ifstream fileJsonConfig("../pipeConfig.json");

    if (!fileJsonConfig.is_open()) {
        cerr << "File from object ifstream \'fileJsonConfig\' not open!" << std::endl;
        throw std::runtime_error{ "FILE CONFIG NOT FOUND" };
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
            return [](TensorT data) { /*std::cout << data[0];*/ return data; };
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

        std::shared_ptr<queueT> rawDataQ{ new queueT };  // buffer

        boost::thread_group networkThreadpool;
        networkThreadpool.create_thread([&ctx, rawDataQ]()
            {
                std::cout << "threadstart" << std::endl;
                zmq::socket_t dataReceiver(ctx, zmq::socket_type::sub);
                dataReceiver.connect("tcp://127.0.0.1:5555");
                dataReceiver.set(zmq::sockopt::subscribe, "");

                std::shared_ptr<zmq::message_t> msg{};
                ShapeT shapeVector{}; shapeVector.reserve(3);

                while (true) {

                    msg.reset(new zmq::message_t);
                    
                    boost::this_thread::interruption_point();

                    zmq::recv_result_t result = dataReceiver.recv(*msg, zmq::recv_flags::dontwait);  // change flag after debugging

                    if (!result)  // instead of assert below
                    {
                        std::this_thread::sleep_for(10ms);    // delete after debugging (and change flag)
                        continue;
                    }
                    
                    auto startTimer = chrono::high_resolution_clock::now();

                    // create a pointer to aligned data from the received message
                    auto alignedPtr = kj::ArrayPtr<const capnp::word>((capnp::word*)msg->data(), msg->size() / sizeof(capnp::word));

                    // create an input stream from the memory buffer
                    capnp::FlatArrayMessageReader message_reader(alignedPtr, capnp::ReaderOptions{9999999999999ui64, 512});
                    NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

                    auto capnProtoTimer = chrono::high_resolution_clock::now();

                    // deserialization and putting data into a tensor
                    // 
                    // shape
                    for (auto&& dimm : ndarray.getShape()) {
                        shapeVector.push_back(static_cast<std::size_t>(dimm));
                    }
                    
                    //
                    // data
                    auto tensorAdapter = xt::adapt_smart_ptr((double*)(&ndarray.getData().asBytes()[0]), shapeVector, msg);
                    std::shared_ptr<TensorT> tensor{ new TensorT { tensorAdapter} };
                    shapeVector.clear();

                    auto prepareTensorTimer = chrono::high_resolution_clock::now();

                    while (!rawDataQ->push(tensor)) {}

                    auto finishTimer = chrono::high_resolution_clock::now();

                    /*std::cout << "Time measures:\n"
                        << "\tCap\'n proto work time -> " << std::chrono::duration<double, milli>(capnProtoTimer - startTimer).count() << "ms\n"
                        << "\tTensor init time -> " << std::chrono::duration<double, milli>(prepareTensorTimer - capnProtoTimer).count() << "ms\n"
                        << "\tTotal time spent -> " << std::chrono::duration<double, milli>(finishTimer - startTimer).count() << "ms" << std::endl;
                    */
                }
            }
        );

        std::cout << "size queue -> " << rawDataQ->size() << std::endl;

        std::cout << "Wait while input queue will filled" << std::endl;
        std::this_thread::sleep_for(6000ms); // work emulation

        std::cout << "size queue -> " << rawDataQ->size() << std::endl;

        // InputNetworkManager {end}
        // 
        // Pipeline -> 'ProcessingManager' {begin}

        // Creating queue for each stage
        Pipeline::Stage<TensorT> stages = getStages();
        std::cout << stages << std::endl;
        std::cout << "ID -> " << stages.childs[1].id << std::endl;

        // create map for stages launch in multi-threading

        std::unordered_map<size_t, Pipeline::Stage<TensorT>> stagesMap; // id ->  stage by id
        std::unordered_map<size_t, std::shared_ptr<queueT>> inputQueueMap; // get parent queue for current stage by id (i.e. get input queue)
        std::unordered_map<size_t, std::vector<std::shared_ptr<queueT>>> outputQueueMap; // get vector of child queues for current stage by id (i.e. get output queues)

        // fill map's
        inputQueueMap.insert({ 0, rawDataQ });

        std::queue<Pipeline::Stage<TensorT>*> queue;
        queue.push(&stages);

        while (!queue.empty()) {

            auto current = queue.front();

            outputQueueMap.insert({ current->id, {} });
            outputQueueMap.at(current->id).reserve(current->childs.size());

            for (auto&& child: current->childs) {

                std::shared_ptr<queueT> temp{ new queueT };
                outputQueueMap.at(current->id).push_back(temp);
                inputQueueMap.insert({ child.id , temp });
                queue.push(&child);
            }

            stagesMap.insert({ current->id, (*current) });

            queue.pop();
        }

        // create threadPool for stages
        boost::thread_group stagesThreadpool;

        // put stage in thread
        for (auto stageIt = stagesMap.begin(); stageIt != stagesMap.end(); ++stageIt) {

            //std::cout << "Stage id -> " << stageIt->second.id << "; Name -> " << stageIt->second.name << std::endl;

            // preparing the necessary objects
            auto&& inputQueue = inputQueueMap.at(stageIt->second.id);
            auto&& outputQueues = outputQueueMap.at(stageIt->second.id);
            auto&& executor = std::move(stageIt->second.callable); // copy or move?? (TODO move)

            // create thread
            stagesThreadpool.create_thread([id = stageIt->first, inputQueue, outputQueues, executor]() {

                while (true) {

                    boost::this_thread::interruption_point();

                    if (inputQueue->size()) {

                        //std::cout << "Thread id -> " << id << "; get work" << std::endl;

                        auto startTimer = chrono::high_resolution_clock::now();

                        std::shared_ptr<TensorT> argData;
                        if (inputQueue->pop(argData)) { // else queue is empty (another thread may have received the value earlier)

                            auto&& res = executor(*argData);
                            auto&& temp = std::shared_ptr<TensorT>{ new TensorT(res) };
                            for (auto&& q : outputQueues) {

                                while (!q->push(temp)) {}
                            }

                            auto finishTimer = chrono::high_resolution_clock::now();
                            //std::cout << "\tThread cycle work time -> " << std::chrono::duration<double, milli>(finishTimer - startTimer).count() << "ms\n" << std::endl;
                        }
                    }
                    else {
                        std::this_thread::sleep_for(5ms);
                    }
                }
                }
            );
        }

        // Pipeline -> 'ProcessingManager' {end}
        //
        // Pipeline -> 'MonitoringManager' {begin}

        // Creating a thread to monitor the operation of the pipeline
        // collects statistics into a json file
        // then sends via cppzmq
        boost::thread monitoringThread([&stagesMap, &inputQueueMap]() {

            const json pattern = {
                { "IdToCallable", {}},
                { "QueueLoad", {}},
                { "ThreadTimePerOp", {}}
            };

            while (true) {

                boost::this_thread::interruption_point();

                json monitoring = pattern;

                auto&& idCallableObj = monitoring["IdToCallable"];
                auto&& queueLoad = monitoring["QueueLoad"];
                auto&& threadTimePerOp = monitoring["ThreadTimePerOp"];

                for (auto it = stagesMap.begin(); it != stagesMap.end(); ++it) {

                    idCallableObj.emplace(std::to_string(it->second.id), it->second.name);                    
                    queueLoad.emplace(std::to_string(it->second.id), inputQueueMap.at(it->second.id)->size());
                }

                cout << "Monitoring thread info:\n" << monitoring.dump(8) << endl;

                std::this_thread::sleep_for(100ms);
            }
            }
        );

        // Pipeline -> 'MonitoringManager' {end}
        // 
        //---------
        // 
        // Part of all Manager's (stopping threadpool)

        std::cout << "Wait end of work" << std::endl;
        std::this_thread::sleep_for(20000ms);

        monitoringThread.interrupt();
        monitoringThread.join();

        networkThreadpool.interrupt_all();  /// interrupts threads
        networkThreadpool.join_all();   // join threads

        stagesThreadpool.interrupt_all();  /// interrupts threads
        stagesThreadpool.join_all();   // join threads
    }

    return 0;
}