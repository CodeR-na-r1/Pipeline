#pragma once

#include <memory>
#include <functional>

#include "../../stageManager/IStageManager.hpp"

#include "../../pipeline/Stage.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT, typename DaoT>
		struct IStageBuilder {

			virtual std::shared_ptr<Stage::IStageManager<DataT, DaoT>> build(std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>>&, std::function<DataT(DaoT)>, std::function<DaoT(DaoT, DataT)>) = 0;

			virtual const Stage::Stage<DataT>& getStages() = 0;
			
			virtual ~IStageBuilder() = default;
		};
	}
}
