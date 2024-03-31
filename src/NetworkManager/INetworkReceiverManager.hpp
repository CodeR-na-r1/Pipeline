#pragma once

namespace Pipeline {

    class INetworkReceiverManager {

    public:

        virtual void connect() = 0;

        virtual bool handleMessage() = 0;
    };
}