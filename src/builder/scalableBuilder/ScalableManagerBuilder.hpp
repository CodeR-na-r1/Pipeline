#pragma once

#include "IScalableManagerBuilder.hpp"

#include "../../scalableManager/ScalableManager.hpp"
#include "ScalableManagerAssembly.hpp"

namespace Pipeline {

	namespace Builder {

		class ScalableManagerBuilder : public IScalableManagerBuilder {

			int countReserveOfThread{ 1 };

		public:

			ScalableManagerBuilder() : IScalableManagerBuilder() {}

			ScalableManagerBuilder& setCountReserveOfThread(int countReserveThreads) {

				countReserveOfThread = countReserveThreads;

				return *this;
			}

			[[nodiscard]]
			std::unique_ptr<IScalableManagerBuilder> build() {

				return std::unique_ptr<IScalableManagerBuilder>{new ScalableManagerBuilder{ std::move(*this) }};
			}

			virtual std::shared_ptr<Scalable::IScalableManager> build(std::unordered_set<std::size_t> scalableProperties, const std::vector<std::pair<std::size_t, std::function<void(void)>>>& stageCallables) override {

				ScalableManagerAssembly smAsm{ countReserveOfThread, scalableProperties, stageCallables };
				smAsm.initMonitoringCallback();
				smAsm.initThreadCallables();

				return std::shared_ptr<Scalable::IScalableManager>{new Scalable::ScalableManager{ smAsm.monitoringCallback, smAsm.threadCallables }};
			}
		};
	}
}
