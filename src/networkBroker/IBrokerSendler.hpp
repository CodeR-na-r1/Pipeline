#pragma once

namespace Pipeline {

    namespace Broker {

        template<typename BrokerOutputT>
        class IBrokerSendler {

        public:

            virtual void connect() = 0;

            virtual bool handleMessage(BrokerOutputT) = 0;

            virtual ~IBrokerSendler() = default;
        };
    }
}