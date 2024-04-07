#pragma once

#include "IConnector.hpp"

#include "cds/container/vyukov_mpmc_cycle_queue.h"

namespace Pipeline {

	namespace Connector {

		template <typename DataT, size_t queueCapacity>
		class MPMCConnector : public IConnector<DataT> {

			using containerT = cds::container::VyukovMPMCCycleQueue<
				DataT, typename cds::container::vyukov_queue::make_traits<
				cds::opt::buffer<
				cds::opt::v::initialized_static_buffer<void*, queueCapacity>>,
				cds::opt::item_counter< cds::atomicity::item_counter>
				>::type>;

			containerT storage;

		public:

			MPMCConnector() :IConnector<DataT>() {}

			virtual bool push(const DataT& data) override {
				return storage.push(data);
			}

			virtual bool hasValue() override {
				return !storage.empty();
			}

			virtual std::size_t size() override {
				return storage.size();
			}

			virtual bool extract(DataT& data) override {
				return storage.pop(data);
			}
		};
	}
}