#pragma once

#include "../../ScalableManager/IScalableManager.hpp"

#include <memory>
#include <functional>
#include <vector>
#include <unordered_set>

namespace Pipeline {

	namespace Builder {

		struct IScalableManagerBuilder {

			virtual std::shared_ptr<Scalable::IScalableManager> build(std::unordered_set<std::size_t>, const std::vector<std::pair<std::size_t, std::function<void(void)>>>&) = 0;
		};
	}
}