#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"

#include "OpencvChooser.hpp"

#include "builder/ReceiverNetworkManagerBuilder.hpp"
#include "networkBroker/ZmqReceiverBroker.hpp"
#include "protoManager/DeserializationManager.hpp"

#include "builder/SendlerNetworkManagerBuilder.hpp"
#include "networkBroker/ZmqSendlerBroker.hpp"
#include "protoManager/SerializationManager.hpp"

#include "src/builder/MonitoringBuilder.hpp"

#include "builder/scalableBuilder/ScalableManagerBuilder.hpp"
#include "builder/scalableBuilder/ScalableStageBuilder.hpp"
#include "builder/scalableBuilder/ScalableParallelPipelineBuilder.hpp"

#include "utils/InputDistributor.hpp"
#include "utils/OutputDistributor.hpp"

#include "detail/monitoring/Measurements.hpp"
#include "detail/monitoring/RelaxedMeasurements.hpp"

using namespace std;

using namespace Pipeline;

/* define traits for pipeline*/

struct UserTraitsScalablePipeline {

    using DataT = cv::Mat;
    using BrokerInputT = Broker::ZmqReceiverBroker::BrokerInputT;
    using BrokerOutputT = Broker::ZmqSendlerBroker::BrokerOutputT;
    using DaoT = Builder::ScalableParallelPipelineBuilder<DataT>::DaoT;

    static constexpr std::size_t ConnectorSize = 1024;
    using MeasurementsT = Pipeline::detail::RelaxedMeasurements;
};

int main() try {

    auto logo = cv::imread(std::filesystem::path{ "../../../examples/resources/logo.jpg" }.generic_string());
    OpencvChooser chooser{ logo };

#ifdef __linux__

    const std::string configFileName{ "../../../examples/scalableDynamicParallelPipeline/ScalableDynamicPipeConfigLinux.json" };

#elif _WIN32

    const std::string configFileName{ "../../../examples/scalableDynamicParallelPipeline/ScalableDynamicPipeConfigWin32.json" };

#endif

    Builder::ScalableParallelPipelineBuilder<UserTraitsScalablePipeline::DataT> pipeBuilder{};

    Pipeline::Utils::inputDistributor<UserTraitsScalablePipeline>(pipeBuilder, configFileName); // for load ReceiverNetworkManager from config file

    Pipeline::Utils::outputDistributor<UserTraitsScalablePipeline>(pipeBuilder, configFileName); // for load SendlerNetworkManager from config file

    pipeBuilder.setStages(
        Builder::ScalableStageBuilder<UserTraitsScalablePipeline>{}
            .addConfigFile(configFileName)  // set config file
            .addChoosers(chooser.callables.cbegin(), chooser.callables.cend())  // set functions for stages
        .build());

    pipeBuilder.addMonitoringManager(
        Builder::MonitoringBuilder<typename UserTraitsScalablePipeline::DaoT>{}
            .addMonitoringCallback([](std::shared_ptr<detail::IMonitoringMeasurements> measures) {
                std::cout << "Queue load:\n";
                for (auto&& it : measures->getQueueLoad()) {
                    std::cout << "\tstage id " << it.first << " ---> " << it.second << " / " << UserTraitsScalablePipeline::ConnectorSize << "\n";
                }
                std::cout << "Time per operand:\n";
                for (auto&& it : measures->getAvgTimePerCallable()) {
                    std::cout << "\tstage id " << it.first << " ---> " << it.second << " ms\n";
                }
                std::cout << "------------\n";
        }).build()
    );

    pipeBuilder.addScalableManager(
        Builder::ScalableManagerBuilder{}
            .setCountReserveOfThread(3) // number of additional threads
        .build()
    );

    auto&& pipeline = pipeBuilder.build();

    std::cout << "Press for start" << std::endl;
    (void)std::getchar();

    pipeline.start();

    std::cout << "Press for stop" << std::endl;
    (void)std::getchar();

    pipeline.stop();

    std::cout << "Press for exit" << std::endl;
    (void)std::getchar();

    return 0;
}
catch (std::exception& e) {

    std::cerr << "Exception: " << e.what() << std::endl;
}