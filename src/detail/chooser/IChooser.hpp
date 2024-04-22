#pragma once

#include <vector>
#include <functional>

namespace Pipeline {

	namespace detail {

		template <typename DataT>
		struct IChooser {

			virtual bool operator()(const std::string&, std::function<DataT(DataT)>&) = 0;

			virtual void addChoser(const std::pair<std::string, std::function<DataT(DataT)>>&) = 0;
			
			virtual ~IChooser() = default;
		};
	}
}
