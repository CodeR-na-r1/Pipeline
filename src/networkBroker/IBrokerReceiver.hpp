#pragma once

namespace Pipeline {

    namespace Broker {

        template<typename BrokerInputT>
        class IBrokerReceiver {

        public:

            virtual void connect() = 0;

            virtual BrokerInputT handleMessage() = 0;
            
            virtual ~IBrokerReceiver() = default;
        };
    }
}
