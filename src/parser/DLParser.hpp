#pragma once

#include "json/json.hpp"

#include <fstream>

#include <vector>
#include <string>

namespace Pipeline {

	namespace Parser {

		using nlohmann::json;

		struct DLParser {

			static std::vector<std::string> getDLProperties(std::ifstream& f) {

				json data = json::parse(f);

				std::vector<std::string> res{};

				if (data.contains("dependencies")) {

					for (auto&& DLFilename : data["dependencies"]) {

						res.push_back(DLFilename.get<std::string>());
					}
				}

				return res;
			}
		};
	}
}