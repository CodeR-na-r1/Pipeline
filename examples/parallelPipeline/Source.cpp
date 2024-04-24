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

#include "builder/parallelBuilder/StageBuilder.hpp"
#include "builder/parallelBuilder/ParallelPipelineBuilder.hpp"

#include "utils/InputDistributor.hpp"
#include "utils/OutputDistributor.hpp"

using namespace std;

using namespace Pipeline;

/* define traits for pipeline*/

struct UserTraitsPipeline {

    using DataT = cv::Mat;
    using BrokerInputT = Broker::ZmqReceiverBroker::BrokerInputT;
    using BrokerOutputT = Broker::ZmqSendlerBroker::BrokerOutputT;
    using DaoT = Builder::ParallelPipelineBuilder<DataT>::DaoT;
};

int main() try {

    /* load resources and functions for stages of pipeline */

    auto logo = cv::imread(std::filesystem::path{ "../../../examples/resources/logo.jpg" }.generic_string());
    OpencvChooser chooser{ logo };

    const std::string configFileName{ "../../../examples/parallelPipeline/PipeConfig.json" };

    Builder::ParallelPipelineBuilder<UserTraitsPipeline::DataT> pipeBuilder{};

    pipeBuilder.addReceiverNetworkManager(
        Builder::ReceiverNetworkManagerBuilder<UserTraitsPipeline>{}
            .setBroker(Broker::ZmqReceiverBroker{ "127.0.0.1", 5555 }.build())
            .setDeserializator(Proto::NDArrayDeserializator<UserTraitsPipeline::DataT>{}.build())
        .build());

    Pipeline::Utils::inputDistributor<UserTraitsPipeline>(pipeBuilder, configFileName); // for load ReceiverNetworkManager from config file

    pipeBuilder.addSendlerNetworkManager(
        Builder::SendlerNetworkManagerBuilder<UserTraitsPipeline>{}
            .addNameStageForConnectManager("display")
            .setSerializator(Proto::NDArraySerializator<UserTraitsPipeline::DataT>{}.build())
            .setBroker(Broker::ZmqSendlerBroker{ "127.0.0.1", 6666 }.build())
        .build()
        );

    Pipeline::Utils::outputDistributor<UserTraitsPipeline>(pipeBuilder, configFileName); // for load SendlerNetworkManager from config file

    pipeBuilder.setStages(
        Builder::StageBuilder<UserTraitsPipeline>{}
            .addConfigFile(configFileName)  // set config file
            .addChoosers(chooser.callables.cbegin(), chooser.callables.cend())  // set functions for stages
        .build());

    pipeBuilder.addMonitoringManager(
        Builder::MonitoringBuilder<typename UserTraitsPipeline::DaoT>{}
            .addMonitoringCallback([](std::shared_ptr<detail::IMonitoringMeasurements> measures) {
                std::cout << "Queue load:\n";
                for (auto&& it : measures->getQueueLoad()) {
                    std::cout << "\tstage id " << it.first << " ---> " << it.second << " / 1024\n";
                }
                std::cout << "------------\n";
        }).build()
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