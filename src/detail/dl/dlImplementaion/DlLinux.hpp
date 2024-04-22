#pragma once

#include <string>
#include <dlfcn.h>

namespace Pipeline {

	namespace DL {

		class DL {

			typedef void*(*MYPROC)(void*);

			void* handle{};

		public:

			DL() = delete;
			DL(const DL&) = delete;

			DL(const std::string& dlFileName) {

				handle = dlopen(dlFileName.c_str(), RTLD_LAZY);
				if (!handle)
					std::cerr << dlerror() << std::endl;
			}

			DL(DL&&) = delete;

			bool operator()(void* arg) {

				if (handle) {
					
					MYPROC ProcAdd = reinterpret_cast<MYPROC>(dlsym(handle, "getChooser"));
					if (ProcAdd != nullptr) {

						(ProcAdd)(arg);
						return true;
					}
				}

				return false;
			}

			~DL()
			{
				dlclose(handle);
			}
		};
	}
}
