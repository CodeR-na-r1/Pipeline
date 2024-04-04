#pragma once

namespace Pipeline {

    namespace Broker {

        class IBrokerReceiver {

        public:

            virtual void connect() = 0;

            virtual bool handleMessage() = 0;
        };
    }
}