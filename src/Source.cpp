#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"

#include "OpencvChooser.hpp"

#include "builder/ReceiverNetworkManagerBuilder.hpp"
#include "networkManager/StandartReceiverNetworkManager.hpp"
#include "networkBroker/ZmqReceiverBroker.hpp"
#include "protoManager/DeserializationManager.hpp"

#include "builder/SendlerNetworkManagerBuilder.hpp"
#include "networkManager/StandartSendlerNetworkManager.hpp"
#include "networkBroker/ZmqSendlerBroker.hpp"
#include "protoManager/SerializationManager.hpp"

#include "src/builder/MonitoringBuilder.hpp"
#include "src/monitoringManager/MonitoringManager.hpp"

// for no scale pipeline

#include "builder/parallelBuilder/StageBuilder.hpp"
#include "builder/parallelBuilder/ParallelPipelineBuilder.hpp"

// for scale pipeline

#include "builder/scalableBuilder/ScalableManagerBuilder.hpp"
#include "builder/scalableBuilder/ScalableStageBuilder.hpp"
#include "builder/scalableBuilder/ScalableParallelPipelineBuilder.hpp"

#include "utils/InputDistributor.hpp"
#include "utils/OutputDistributor.hpp"

#include "detail/monitoring/Measurements.hpp"

using namespace std;

using namespace Pipeline;

struct UserTraitsPipeline {

    using DataT = cv::Mat;
    using BrokerInputT = Broker::ZmqReceiverBroker::BrokerInputT;
    using BrokerOutputT = Broker::ZmqSendlerBroker::BrokerOutputT;
    using DaoT = Builder::ParallelPipelineBuilder<DataT>::DaoT;

    static constexpr std::size_t ConnectorSize = 1024;
    using MeasurementsT = Pipeline::detail::Measurements;
};

struct UserTraitsScalablePipeline {

    using DataT = cv::Mat;
    using BrokerInputT = Broker::ZmqReceiverBroker::BrokerInputT;
    using BrokerOutputT = Broker::ZmqSendlerBroker::BrokerOutputT;
    using DaoT = Builder::ScalableParallelPipelineBuilder<DataT>::DaoT;

    static constexpr std::size_t ConnectorSize = 1024;
    using MeasurementsT = Pipeline::detail::Measurements;
};

//#define PARALLEL_PIPE_TYPE

#define SCALABLE_PARALLEL_PIPE_TYPE

int main() try {

#ifdef PARALLEL_PIPE_TYPE

    // no scale pipeline
    {
        auto logo = cv::imread(std::filesystem::path{ "../../examples/resources/logo.jpg" }.generic_string());
        OpencvChooser chooser{ logo };

        const std::string configFileName{ "../../src/PipeConfig.json" };

        Builder::ParallelPipelineBuilder<UserTraitsPipeline::DataT> pipeBuilder{};

        pipeBuilder.addReceiverNetworkManager(
            Builder::ReceiverNetworkManagerBuilder<UserTraitsPipeline>{}
            .setBroker(Broker::ZmqReceiverBroker{ "127.0.0.1", 5555 }.build())
            .setDeserializator(Proto::NDArrayDeserializator<UserTraitsPipeline::DataT>{}.build())
            .build());

        Pipeline::Utils::inputDistributor<UserTraitsPipeline>(pipeBuilder, configFileName);

        pipeBuilder.addSendlerNetworkManager(
            Builder::SendlerNetworkManagerBuilder<UserTraitsPipeline>{}
                .addNameStageForConnectManager("storeTwo")
                .setSerializator(Proto::NDArraySerializator<UserTraitsPipeline::DataT>{}.build())
                .setBroker(Broker::ZmqSendlerBroker{ "127.0.0.1", 6666 }.build())
                .build()
        );

        Pipeline::Utils::outputDistributor<UserTraitsPipeline>(pipeBuilder, configFileName);

        pipeBuilder.setStages(
            Builder::StageBuilder<UserTraitsPipeline>{}
                .addConfigFile(configFileName)
                .addChoosers(chooser.callables.cbegin(), chooser.callables.cend())
            .build());

        pipeBuilder.addMonitoringManager(
            Builder::MonitoringBuilder<typename UserTraitsPipeline::DaoT>{}
                .addMonitoringCallback([](std::shared_ptr<detail::IMonitoringMeasurements> measures) {
                    std::cout << "Queue load:\n";
                    for (auto&& it : measures->getQueueLoad()) {
                        std::cout << "\tstage id " << it.first << " ---> " << it.second << " / " << UserTraitsPipeline::ConnectorSize << "\n";
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
    }

#endif // PARALLEL_PIPE_TYPE

#ifdef SCALABLE_PARALLEL_PIPE_TYPE

    // scale pipeline
    {
        auto logo = cv::imread(std::filesystem::path{ "../../examples/resources/logo.jpg" }.generic_string());
        OpencvChooser chooser{ logo };

        const std::string configFileName{ "../../src/ScalableDynamicPipeConfig.json" };

        Builder::ScalableParallelPipelineBuilder<UserTraitsScalablePipeline::DataT> pipeBuilder{};

        pipeBuilder.addReceiverNetworkManager(
            Builder::ReceiverNetworkManagerBuilder<UserTraitsScalablePipeline>{}
        .setBroker(Broker::ZmqReceiverBroker{ "127.0.0.1", 5555 }.build())
            .setDeserializator(Proto::NDArrayDeserializator<UserTraitsScalablePipeline::DataT>{}.build())
            .build());

        Pipeline::Utils::inputDistributor<UserTraitsScalablePipeline>(pipeBuilder, configFileName);

        pipeBuilder.addSendlerNetworkManager(
            Builder::SendlerNetworkManagerBuilder<UserTraitsScalablePipeline>{}
                .addNameStageForConnectManager("display")
                .setSerializator(Proto::NDArraySerializator<UserTraitsScalablePipeline::DataT>{}.build())
                .setBroker(Broker::ZmqSendlerBroker{ "127.0.0.1", 6666 }.build())
            .build()
        );

        Pipeline::Utils::outputDistributor<UserTraitsScalablePipeline>(pipeBuilder, configFileName);

        pipeBuilder.setStages(
            Builder::ScalableStageBuilder<UserTraitsScalablePipeline>{}
                .addConfigFile(configFileName)
                .addChoosers(chooser.callables.cbegin(), chooser.callables.cend())
            .build());

        pipeBuilder.addMonitoringManager(
            Builder::MonitoringBuilder<typename UserTraitsScalablePipeline::DaoT>{}
        .addMonitoringCallback([](std::shared_ptr<detail::IMonitoringMeasurements> measures) {
            std::cout << "Queue load:\n";
            for (auto&& it : measures->getQueueLoad()) {
                std::cout << "\tstage id " << it.first << " ---> " << it.second << " / " << UserTraitsScalablePipeline::ConnectorSize << "\n";
            }
            std::cout << "------------\n";
            }).build()
                );

        pipeBuilder.addScalableManager(
            Builder::ScalableManagerBuilder{}
            .setCountReserveOfThread(3).build()
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
    }

#endif // SCALABLE_PARALLEL_PIPE_TYPE

    return 0;
}
catch (std::exception& e) {

    std::cerr << "Exception: " << e.what() << std::endl;
}
