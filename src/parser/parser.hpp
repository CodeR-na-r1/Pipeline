#pragma once

#include "pipeline/stage.hpp"
#include "json/json.hpp"

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <queue>

#include <optional>
#include <functional>

namespace Pipeline {

	using nlohmann::json;
			
	struct JsonParser {

		//static auto findChilds = []() {};

		template <typename TData>
		static std::optional<Stage<TData>> fromFile(std::ifstream& f, std::function<std::function<TData(TData)>(std::string)> chooser) {
		
			json data = json::parse(f);

			json stagesInfo = data["stages"];
			json stages = stagesInfo["list"];

			json connections = data["connections"]["list"];

			std::string startName = stagesInfo["start"];
			auto startCallable = std::find_if(stages.begin(), stages.end(), [&startName](auto a) { return a["name"] == startName; });
			if (startCallable == stages.end()) {
				return std::nullopt;
			}

			Stage res{ chooser((*startCallable)["callable"]), startName, {} };

			std::queue<Stage<TData>*> queue;
			queue.push(&res);

			while (!queue.empty()) {

				auto current = queue.front();

				auto temp = std::find_if(connections.begin(), connections.end(), [name = current->name](auto a) { return a["from"] == name; });
				if (temp != connections.end()) {

					json value = (*temp)["to"];
					if (value.is_array()) {
						for (auto&& child : value) {
							auto callableNameIt = std::find_if(stages.begin(), stages.end(), [child](auto a) { return a["name"] == child; });
							if (callableNameIt == stages.end()) {
								return std::nullopt;
							}
							current->childs.push_back({ chooser((*callableNameIt)["callable"]), child, {} });
						}
						for (auto&& child : current->childs) {
							queue.push(&child);
						}
					}
					//else if (value.is_string()) {

					//}
					else {
						return std::nullopt;
					}
				}
				//auto connectionInfo = std::find();

				// TODO: impl beautiful find child in json object
				// TODO: create childs and push ref on him in queue

				//for (auto&& it: connections) {

					//current.childs.push_back(child);
				//}

				// code here;

				queue.pop();
			}

			return res;
		}

		//template <>
		//static Pipeline<> fromFile() {
		
		//}
	};

}