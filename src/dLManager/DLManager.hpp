#pragma once

#include "IDLManager.hpp"

#include <memory>
#include <vector>

#include "../detail/dl/DL.hpp"

namespace Pipeline {

	namespace DL {

		class DLManager : public IDLManager {

			std::vector<std::shared_ptr<DL>> DLs{};

		public:

			DLManager(std::vector<std::shared_ptr<DL>> DLs) : IDLManager(), DLs(DLs) {}
		};
	}
}