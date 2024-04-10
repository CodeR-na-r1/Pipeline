#pragma once

namespace Pipeline {

    namespace Broker {

        template<typename BrokerT>
        class IBrokerReceiver {

        public:

            virtual void connect() = 0;

            virtual BrokerT handleMessage() = 0;
        };
    }
}