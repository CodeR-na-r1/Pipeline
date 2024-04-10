#pragma once

#include <memory>
#include <functional>

#include "../../StageManager/IStageManager.hpp"

namespace Pipeline {

	namespace Builder {

		template <typename DataT, typename DaoT>
		struct IStageBuilder {

			virtual std::shared_ptr<Stage::IStageManager<DataT, DaoT>> build(std::function<DataT(DaoT)>, std::function<DaoT(DaoT, DataT)>) = 0;
		};
	}
}