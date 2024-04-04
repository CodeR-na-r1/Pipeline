#pragma once

#include "IConnector.hpp"

#include "boost/lockfree/spsc_queue.hpp"

namespace Pipeline {

	namespace Connector {

		template <typename DataT, size_t queueCapacity>
		class SPSCConnector : public IConnector<DataT> {

			using containerT = boost::lockfree::spsc_queue<DataT, boost::lockfree::capacity<queueCapacity>>;

			containerT storage;

		public:

			SPSCConnector() :IConnector<DataT>() {}

			virtual bool push(const DataT& data) override {
				return storage.push(data);
			}

			virtual bool hasValue() override {
				return storage.read_available();
			}

			virtual std::size_t size() override {
				return storage.read_available();
			}

			virtual bool extract(DataT& data) override {

				if (hasValue()) {
					data = storage.front();
				}

				return storage.pop();
			}
		};
	}
}