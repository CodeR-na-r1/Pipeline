#pragma once

#include <iostream>

#include <string>
#include <vector>

#include <functional>

namespace Pipeline {

	namespace Stage {

		template <typename TData>
		struct Stage {

		private:

			static std::size_t idTotalCounter;

			std::size_t id;
			std::function<TData(TData)> callable;
			std::string name;
			std::vector<Stage<TData>> childs;

		public:

			Stage() = delete;

			Stage(std::function<TData(TData)> callable, std::string name, std::vector<Stage<TData>> childs) : id(idTotalCounter++), callable(callable), name(name), childs(childs) {}

			Stage(const Stage& other) : id(idTotalCounter++), callable(other.callable), name(other.name), childs(other.childs) {}

			Stage(Stage&&) = default;

			void operator()(TData data) {

				auto result = callable(data);

				for (auto&& child : childs) {

					child(result);
				}
			}

			const std::size_t getId() const {
				return id;
			}

			const std::string getName() const {
				return name;
			}

			const std::function<TData(TData)> getCallable() const {
				return callable;
			}

			const std::vector<Stage<TData>>& getChilds() const {
				return childs;
			}

			std::vector<Stage<TData>>& getChilds() {
				return childs;
			}

			void addChild(const Stage<TData>& child) {

				childs.push_back(child);
			}

			void addChild(Stage<TData>&& child) {

				childs.push_back(std::move(child));
			}

			void dump(std::ostream& os) const {

				os << name << "(";

				bool isFirst = true;	// for beautiful output

				for (auto&& child : childs) {

					if (!isFirst)
						os << ", ";
					os << child;
					isFirst = false;
				}
				os << ")";
			}

			friend std::ostream& operator<<(std::ostream& os, const Stage s) {

				s.dump(os);

				return os;
			}

		};

		template<typename TData>
		std::size_t Stage<TData>::idTotalCounter = 0;
	}
}