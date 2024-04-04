#pragma once

#include <cstddef>
#include <memory>

namespace Pipeline {

	namespace Connector {

		template <typename DataT>
		class IConnector {

		public:

			virtual bool push(const DataT& data) = 0;

			virtual bool hasValue() = 0;

			virtual std::size_t size() = 0;

			virtual bool extract(DataT& data) = 0;

		};

	}
}