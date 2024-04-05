#pragma once

#include "IChooser.hpp"

#include <vector>
#include <functional>

namespace Pipeline {

	namespace detail {

		template <typename DataT>
		class Chooser : public IChooser<DataT> {

			std::vector<std::pair<std::string, std::function<DataT(DataT)>>> choosers;

		public:

			Chooser() : IChooser<DataT>() {}

			virtual bool operator()(const std::string& callableName, std::function<DataT(DataT)>& callable) override {

				for (auto&& chooser : choosers) {

					if (chooser.first == callableName) {
						callable = chooser.second;
						return true;
					}
				}

				return false;
			}

			virtual void addChoser(const std::pair<std::string, std::function<DataT(DataT)>>& chooser) override {

				choosers.push_back(chooser);
			}
		};
	}
}