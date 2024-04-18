#pragma once

#include "ISendlerNetworkManager.hpp"

#include <functional>

namespace Pipeline {

	namespace Network {

		class StandartSendlerNetworkManager : public ISendlerNetworkManager {

			std::function<void(void)> callable{};

		public:

			StandartSendlerNetworkManager() = delete;

			StandartSendlerNetworkManager(std::function<void(void)> callable) : callable(callable) {}

			virtual void operator()() override {

				callable();
			}
		};
	}
}