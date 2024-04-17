#pragma once

#include <string>

#include <windows.h> 
#include <stdio.h> 

namespace Pipeline {

	namespace DL {

		typedef void(__cdecl* MYPROC)(void*);

		class DL {

			HINSTANCE hinstLib;

		public:

			DL() = delete;
			DL(const DL&) = delete;

			DL(const std::string& dlFileName) {

				hinstLib = LoadLibrary(TEXT(dlFileName.c_str()));
			}

			DL(DL&&) = default;

			bool operator()(void* arg) {

				if (hinstLib != nullptr) {

					MYPROC ProcAdd = (MYPROC)GetProcAddress(hinstLib, "getChooser");

					if (ProcAdd != nullptr) {

						(ProcAdd)(arg);
						return true;
					}
				}

				return false;
			}

			~DL()
			{
				FreeLibrary(hinstLib);
			}
		};
	}
}