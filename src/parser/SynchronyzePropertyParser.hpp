#pragma once

#include "json/json.hpp"

#include "../pipeline/Stage.hpp"
#include "../detail/functions/StageFunctions.hpp"

#include <fstream>

#include <unordered_set>
#include <optional>

namespace Pipeline {

	namespace Parser {

		using nlohmann::json;

		struct SynchronyzePropertiesParser {

			template <typename TData>
			static std::optional<std::unordered_set<std::size_t>> getSynchronyzeProperties(std::ifstream& f, const Stage::Stage<TData>& stages) {

				json data = json::parse(f);

				json stagesInfo = data["stages"]["list"];

				std::unordered_set<std::size_t> res;

				for (auto&& stage : stagesInfo) {

					if (stage.contains("isSync")) {

						auto&& id = detail::nameToId(stage["name"]);

						if (id.has_value())
							res.insert(id.value());
						else
							return {};
					}
				}

				return res;
			}
		};
	}
}