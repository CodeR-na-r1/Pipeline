#pragma once

#include "../../pipeline/Stage.hpp"

#include <string>
#include <optional>

namespace Pipeline {

	namespace detail {

		template <typename TData>
		std::optional<std::size_t> nameToId(const Stage::Stage<TData>& stages, const std::string& name) {

			if (stages.getName() == name)
				return stages.getId();

			for (auto&& child : stages.getChilds()) {

				auto&& res = nameToId(child, name);
				if (res.has_value())
					return res;
			}

			return {};
		}
	}
}