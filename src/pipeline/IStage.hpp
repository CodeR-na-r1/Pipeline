#pragma once

#include <string>
#include <vector>

#include <functional>

namespace Pipeline {

	namespace Stage {

		template <typename TData>
		struct IStage {

			virtual const std::size_t getId() const = 0;

			virtual const std::string getName() const = 0;

			virtual const std::function<TData(TData)> getCallable() const = 0;

			virtual void addChild(Stage<TData>) = 0;

			virtual const std::vector<Stage<TData>> getChilds() const = 0;

			virtual std::vector<Stage<TData>> getChilds() = 0;
		};
	}
}