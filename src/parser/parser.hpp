#pragma once

#include "pipeline/stage.hpp"
#include "json/json.hpp"

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <queue>

#include <functional>

namespace Pipeline {

	using nlohmann::json;
			
	struct JsonParser {

		template <typename TData>
		static Stage<TData> fromFile(std::ifstream& f, std::function<std::function<TData(TData)>(std::string)> chooser) {
		
			json data = json::parse(f);

			json stages = data["stages"];
			std::string startName = stages["start"];

			json connections = data["connections"]["list"];

			Stage res{ chooser(startName), startName, {} };
			std::queue<Stage<TData>*> queue;
			queue.push(&res);

			while (!queue.empty()) {

				auto current = queue.front();
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
			//return { {},{},{} };
		}

		//template <>
		//static Pipeline<> fromFile() {
		
		//}
	};

}