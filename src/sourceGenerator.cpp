// external headers

#include <iostream>
#include <filesystem>

#include <memory>
#include <chrono>
#include <thread>
#include <stdexcept>

#include <vector>
#include <unordered_map>

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "boost/asio.hpp"
#include "boost/asio/thread_pool.hpp"
#include <boost/thread/thread.hpp>

#include "boost/lockfree/spsc_queue.hpp"

#include <xtensor/xarray.hpp>

#include "json/json.hpp"

#include "opencv2/opencv.hpp"

#include "cds/container/vyukov_mpmc_cycle_queue.h"

// internal headers

#include "pipeline/stage.hpp"
#include "parser/parser.hpp"
#include "pipeline/pipeline.hpp"

#include "detail/monitoring/Measurements.hpp"

#include "ProtoManager/DeserializationManager.hpp"
#include "NetworkManager/InetworkReceiverManager.hpp"
#include "NetworkManager/ZmqReceiverManager.hpp"

// 'user' headers

#include "opencvChooser.hpp"

using namespace std;
using namespace chrono_literals;

using Pipeline::detail::Measurements;

using json = nlohmann::json;

using DataT = std::uint8_t;
//using TensorT = xt::xarray<DataT>;
using TensorT = cv::Mat;
using ShapeT = xt::svector<size_t>;

//using StageDataT = std::shared_ptr<TensorT>;
using StageDataT = TensorT;

using MPMCQueue = cds::container::VyukovMPMCCycleQueue<
    StageDataT, typename cds::container::vyukov_queue::make_traits<
        cds::opt::buffer<
            cds::opt::v::initialized_static_buffer<void*, 1024>>,
        cds::opt::item_counter< cds::atomicity::item_counter>
    >::type>;

using spsc_queueT = boost::lockfree::spsc_queue<std::shared_ptr<TensorT>, boost::lockfree::capacity<1024>>;

using queueT = MPMCQueue;

Pipeline::Stage<StageDataT> getStages() {

    ifstream fileJsonConfig("../../src/pipeConfig.json");

    if (!fileJsonConfig.is_open()) {
        cerr << "File from object ifstream \'fileJsonConfig\' not open!" << std::endl;
        throw std::runtime_error{ "FILE CONFIG NOT FOUND" };
    }

    auto logo = cv::imread(std::filesystem::path{ "../../examples/resources/logo.jpg" }.generic_string());
    OpencvChooser chooser{ logo };

    Pipeline::Stage<StageDataT> source = Pipeline::JsonParser::fromFile<StageDataT>(fileJsonConfig, chooser).value_or(Pipeline::Stage<StageDataT>{ [](StageDataT data) { return data; }, {}, {} });

    fileJsonConfig.close();

    return source;
}

struct ZmqNetworkManagerTraits {

    using DataT = ::DataT;
    using QueueT = queueT;
    using TensorT = ::TensorT;
    using DeserializerT = Pipeline::NDArrayDeserializator<TensorT>;
};

int main() {

    {
        std::shared_ptr<queueT> rawDataQ{ new queueT };  // buffer

        boost::thread_group networkThreadpool;
        networkThreadpool.create_thread([rawDataQ]()
            {   
                Pipeline::ZmqReceiverManager<ZmqNetworkManagerTraits> zmqManager{ "127.0.0.1", 5558, rawDataQ };

                zmqManager.connect();

                while (true) {
                
                    boost::this_thread::interruption_point();

                    auto startTimer = chrono::high_resolution_clock::now();

                    if (zmqManager.handleMessage()) {

                        auto finishTimer = chrono::high_resolution_clock::now();

                        std::cout << "Time measures:\n"
                            << "\tTime for receive packet spent -> " << std::chrono::duration<double, milli>(finishTimer - startTimer).count() << "ms" << std::endl;
                    }
                }
            }
        );

        std::cout << "Wait while input queue will filled" << std::endl;
        (void)std::getchar();
        //std::this_thread::sleep_for(6000ms); // work emulation
        std::cout << "size queue -> " << rawDataQ->size() << std::endl;

        // Pipeline -> 'ProcessingManager' {begin}

        Pipeline::Stage<StageDataT> stages = getStages();
        std::cout << stages << std::endl;
        std::cout << "ID -> " << stages.childs[0].id << std::endl;

        // create map for stages launch in multi-threading

        std::unordered_map<size_t, Pipeline::Stage<StageDataT>> stagesMap; // id ->  stage by id
        std::unordered_map<size_t, std::shared_ptr<queueT>> inputQueueMap; // get parent queue for current stage by id (i.e. get input queue)
        std::unordered_map<size_t, std::vector<std::shared_ptr<queueT>>> outputQueueMap; // get vector of child queues for current stage by id (i.e. get output queues)
        
        std::unordered_map<size_t, std::shared_ptr<Measurements>> threadMeasurementsMap;    // shared_ptr because mutex is not copyable

        // fill map's
        inputQueueMap.insert({ 0, rawDataQ });

        std::queue<Pipeline::Stage<StageDataT>*> queue;
        queue.push(&stages);

        while (!queue.empty()) {

            auto current = queue.front();

            threadMeasurementsMap.insert({ current->id, std::shared_ptr<Measurements>{new Measurements{}} });

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
            auto&& measurements = threadMeasurementsMap.at(stageIt->second.id);

            // create thread
            stagesThreadpool.create_thread([id = stageIt->first, inputQueue, outputQueues, executor, measurements]() {

                while (true) {

                    boost::this_thread::interruption_point();

                    if (inputQueue->size()) {

                        //std::cout << "Thread id -> " << id << "; get work" << std::endl;

                        auto startTimer = chrono::high_resolution_clock::now();

                        StageDataT argData;
                        if (inputQueue->pop(argData)) { // else queue is empty (another thread may have received the value earlier)

                            auto&& res = executor(argData);
                            for (auto&& q : outputQueues) {

                                while (!q->push(res)) {}
                            }

                            auto finishTimer = chrono::high_resolution_clock::now();

                            measurements->push(std::chrono::duration<double, milli>(finishTimer - startTimer).count());

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
        boost::thread monitoringThread([&stagesMap, &inputQueueMap, &threadMeasurementsMap]() {

            json pattern = {
                { "IdToCallable", {}},
                { "QueueLoad", {}},
                { "ThreadTimePerOp", {}}
            };

            for (auto it = stagesMap.begin(); it != stagesMap.end(); ++it) {
                pattern["IdToCallable"].emplace(std::to_string(it->second.id), it->second.name);
            }

            while (true) {

                boost::this_thread::interruption_point();

                json monitoring = pattern;

                auto&& queueLoad = monitoring["QueueLoad"];
                auto&& threadTimePerOp = monitoring["ThreadTimePerOp"];

                for (auto it = stagesMap.begin(); it != stagesMap.end(); ++it) {

                    queueLoad.emplace(std::to_string(it->second.id), inputQueueMap.at(it->second.id)->size());
                    threadTimePerOp.emplace(std::to_string(it->second.id), threadMeasurementsMap.at(it->second.id)->pull());
                }

                cout << "Monitoring thread info:\n" << monitoring.dump(8) << endl;

                std::this_thread::sleep_for(1000ms);
            }
            }
        );

        // Pipeline -> 'MonitoringManager' {end}
        // 
        //---------
        // 
        // Part of all Manager's (stopping threadpool)

        std::cout << "Wait end of work" << std::endl;
        (void)std::getchar();
        //std::this_thread::sleep_for(20000ms);

        monitoringThread.interrupt();
        monitoringThread.join();

        networkThreadpool.interrupt_all();  /// interrupts threads
        networkThreadpool.join_all();   // join threads

        stagesThreadpool.interrupt_all();  /// interrupts threads
        stagesThreadpool.join_all();   // join threads
    }

    return 0;
}