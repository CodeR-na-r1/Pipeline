#include <iostream>
#include <fstream>

#include <conio.h>

#include <vector>

#include <memory>
#include <chrono>

#include "boost/thread/thread.hpp"

#include "zmq.hpp"
#include "zmq_addon.hpp"

#include "../scheme/ndarray.capnp.h"
#include "capnp/serialize.h"
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <kj/std/iostream.h>

#include "IInitializator.hpp"
#include "opencvInitializator.hpp"
#include "xtensorInitializator.hpp"
#include "utensorInitializator.hpp"

using namespace std::chrono_literals;

using DataT = double;

int main() {

    std::vector<std::vector<double>> measurements;

    {
        std::vector<std::unique_ptr<Benchmark::Initializator::IInitializer<DataT>>> initializers{};

        initializers.emplace_back(new Benchmark::Initializator::UtensorInitializer<DataT>);
        initializers.emplace_back(new Benchmark::Initializator::XtensorInitializer<DataT>);
        //initializers.emplace_back(new Benchmark::Initializator::OpencvInitializer<DataT>);
        
        zmq::context_t ctx;

        boost::thread worker{ [&measurements, &initializers, &ctx]
            {
                std::cout << "Benchmark started!" << std::endl;

                zmq::socket_t dataReceiver(ctx, zmq::socket_type::sub);
                dataReceiver.connect("tcp://127.0.0.1:5555");
                dataReceiver.set(zmq::sockopt::subscribe, "");

                std::shared_ptr<zmq::message_t> msg{};

                uint64_t counterIter{};
                while (true) {

                    // receive message
                    msg.reset(new zmq::message_t);

                    boost::this_thread::interruption_point();

                    zmq::recv_result_t result = dataReceiver.recv(*msg, zmq::recv_flags::dontwait);

                    if (!result)
                    {
                        std::this_thread::sleep_for(10ms);
                        continue;
                    }

                    std::cout << "Iteration -> " << ++counterIter << std::endl;

                    // unpacking message via Cap'n Proto
                    auto alignedPtr = kj::ArrayPtr<const capnp::word>((capnp::word*)msg->data(), msg->size() / sizeof(capnp::word));
                    capnp::FlatArrayMessageReader message_reader(alignedPtr, capnp::ReaderOptions{ 9999999999999ui64, 512 });
                    NDArray::Reader ndarray = message_reader.getRoot<NDArray>();

                    measurements.push_back({});
                    auto indexRecord = measurements.size() - 1;

                    auto szMsg = ndarray.getData().size();
                    measurements[indexRecord].push_back(szMsg);   // save data size in message

                    // Benchmark for Initializers
                    for (auto&& init : initializers) {
                        
                        auto time = init->bench(msg, ndarray);

                        std::cout << "\t" << init->getData(1) << std::endl;

                        measurements[indexRecord].push_back(time);  // save time for each initializer
                    }
                }
            }
        };

        std::cout << "Enter any key for stop benchmark...";

        getch();

        worker.interrupt();
        worker.join();

        std::cout << "\nInformation about record in file:" << "\n\tRows - record for each test\n" << "\tCols - measures:\n\t\t1 col = count of elements in message" << std::endl;
        std::size_t counter{ 2 };
        for (auto&& init : initializers) {
            std::cout << "\t\t" << counter++ << " col = time for " << typeid(*init).name() << std::endl;
        }
    }

    std::ofstream file("initMeasurements.txt");

    if (!file.is_open()) {

        std::cerr << "Error open file for record of result" << std::endl;
    }

    for (auto&& measurement : measurements) {

        bool isFirst{ true };
        for (auto&& record : measurement) {

            if (!isFirst) file << "\t";
            file << record;
            isFirst = false;
        }
        file << std::endl;
    }

    file.close();

    std::cout << "Results saved to file!" << std::endl;

    return 0;
}