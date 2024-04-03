#pragma once

#include <unordered_map>
#include <mutex>

namespace Pipeline {

	namespace detail {

		// add concept on idT - целый тип
		template <typename idT, typename DataT>
		class SyncMapConnector {

			idT idCounter{};
			std::unordered_map<idT, DataT> storage{};

			std::mutex guard{};

		public:

			SyncMapConnector(std::uint64_t reserveCount) {

				storage.reserve(reserveCount);
			}

			template <typename ... Args>
			decltype(auto) insert(Args&& ... args) {

				guard.lock();
				decltype(auto) ret = storage.insert(std::forward<Args>(args)...);
				guard.unlock();

				return ret;
			}

			decltype(auto) hasValue() {

				guard.lock();
				decltype(auto) ret = storage.find(idCounter) != storage.end();
				guard.unlock();

				return ret;
			}

			decltype(auto) size() {

				guard.lock();
				decltype(auto) res = storage.size();
				guard.unlock();

				return res;
			}

			template<typename Value>
			bool extract(Value& value) {

				bool succ = false;

				guard.lock();

				decltype(auto) elemIt = storage.find(idCounter);
				if (elemIt != storage.end()) {
					value = std::move(elemIt->second);
					storage.erase(elemIt);
					succ = true;
				}

				guard.unlock();

				if (succ)
					++idCounter;

				return succ;
			}
		};
	}
}