#pragma once

#include <iostream>

#include <string>
#include <vector>

#include <functional>

namespace Pipeline {

	using namespace std;

	template <typename TData>
	struct Stage {

	private:

		static std::size_t idTotalCounter;

	public:

		std::size_t id;
		function<TData(TData)> callable;
		string name;
		vector<Stage> childs;

		Stage(function<TData(TData)> callable, string name, vector<Stage> childs) : id(idTotalCounter++), callable(callable), name(name), childs(childs) {}

		Stage(const Stage& other) = default;

		void operator()(TData data) {

			auto result = callable(data);

			for (auto&& child : childs) {

				child(result);
			}
		}

		void addChild(const Stage child) {

			childs.push_back(child);
		}

		void dump(ostream& os) const {

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

		friend ostream& operator<<(ostream& os, const Stage s) {

			s.dump(os);

			return os;
		}

	};

	template<typename TData>
	std::size_t Stage<TData>::idTotalCounter = 0;
}