#pragma once

#include <string>

namespace Pipeline {

	namespace DL {

		class DL {

		public:

			DL() = delete;
			DL(const DL&) = delete;

			DL(const std::string& dlFileName) {

				
			}

			DL(DL&&) = delete;

			bool operator()(void* arg) {

				return false;
			}

			~DL()
			{
			
			}
		};
	}
}
