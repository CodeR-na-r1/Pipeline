#pragma once

#include <iostream>

#include <string>
#include <vector>

#include <functional>

namespace Pipeline {

	using namespace std;

	template <typename TData>
	struct Stage {

		function<TData(TData)> callable;
		string name;
		vector<Stage> childs;

		Stage(function<TData(TData)> callable, string name, vector<Stage> childs) : callable(callable), name(name), childs(childs) {}

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
			cout << ")";
		}

		friend ostream& operator<<(ostream& os, const Stage s) {

			s.dump(os);

			return os;
		}

	};
}