#pragma once

#include "json/json.hpp"

#include "../pipeline/Stage.hpp"
#include "../detail/chooser/IChooser.hpp"

#include <iostream>
#include <fstream>
#include <memory>

#include <string>
#include <vector>
#include <queue>

#include <optional>
#include <functional>

namespace Pipeline {

	namespace Parser {

		using nlohmann::json;

		struct JsonParser {

			template <typename TData>
			static std::optional<Stage::Stage<TData>> fromFile(std::ifstream& f, std::shared_ptr<detail::IChooser<TData>> chooser) {

				std::function<TData(TData)> tempCallable;

				json data = json::parse(f);

				json stagesInfo = data["stages"];
				json stages = stagesInfo["list"];

				json connections = data["connections"]["list"];

				std::string startName = stagesInfo["start"];
				auto startCallable = std::find_if(stages.begin(), stages.end(), [&startName](auto a) { return a["name"] == startName; });
				if (startCallable == stages.end()) {
					return {};
				}

				if (!chooser->operator()((*startCallable)["callable"], tempCallable))
					return {};

				Stage::Stage<TData> res{ std::move(tempCallable), startName, {} };

				std::queue<Stage::Stage<TData>*> queue;
				queue.push(&res);

				while (!queue.empty()) {

					auto current = queue.front();

					auto temp = std::find_if(connections.begin(), connections.end(), [name = current->getName()](auto a) { return a["from"] == name; });
					if (temp != connections.end()) {

						json value = (*temp)["to"];
						if (value.is_array()) {
							for (auto&& child : value) {
								auto callableNameIt = std::find_if(stages.begin(), stages.end(), [child](auto a) { return a["name"] == child; });
								if (callableNameIt == stages.end()) {
									return {};
								}
								if (!chooser->operator()((*callableNameIt)["callable"], tempCallable))
									return {};
								current->addChild(std::move(Stage::Stage<TData>{ std::move(tempCallable), child, {} }));
							}
							for (auto&& child : current->getChilds()) {
								queue.push(&child);
							}
						}
						else if (value.is_string()) {
							auto callableNameIt = std::find_if(stages.begin(), stages.end(), [value](auto a) { return a["name"] == value; });
							if (callableNameIt == stages.end()) {
								return {};
							}
							if (!chooser->operator()((*callableNameIt)["callable"], tempCallable))
								return {};
							current->addChild(std::move(Stage::Stage<TData>{ std::move(tempCallable), value, {} }));
							queue.push(&(current->getChilds()[0]));
						}
						else {
							return {};
						}
					}

					queue.pop();
				}

				return res;
			}
		};
	}
}