#pragma once

#include <memory>
#include <functional>

#include <unordered_set>

#include "../../stageManager/IStageManager.hpp"

#include "../../pipeline/Stage.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT, typename DaoT>
		struct IScalableStageBuilder {

			virtual IScalableStageBuilder& addChooser(const std::pair<std::string, std::function<DataT(DataT)>>&) = 0;

			virtual std::shared_ptr<Stage::IStageManager<DataT, DaoT>>build(bool, std::function<DataT(DaoT)>, std::function<DaoT(DaoT, DataT)>, std::vector<std::pair<std::string, std::shared_ptr<Connector::IConnector<DaoT>>>>&) = 0;

			virtual const Stage::Stage<DataT>& getStages() = 0;

			virtual std::unordered_set<std::size_t> getScalableProperties() = 0;

			virtual const std::string& getConfigFileName() = 0;
			
			virtual ~IScalableStageBuilder() = default;
		};
	}
}
