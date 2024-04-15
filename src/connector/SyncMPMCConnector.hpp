#pragma once

#include "IConnector.hpp"

#include "../detail/connector/SyncMapConnector.hpp"

#include "pipeline/frame/IDFrame.hpp"

namespace Pipeline {

	namespace Connector {

		template <typename DataT, size_t queueCapacity>
		class SyncMapConnector : public IConnector<DataT> { };


		template <typename IDType, typename DataT, size_t queueCapacity>
		class SyncMapConnector<std::shared_ptr<Frame::IDFrame<IDType, DataT>>, queueCapacity> : public IConnector<std::shared_ptr<Frame::IDFrame<IDType, DataT>>> {

			using FrameDataT = std::shared_ptr<Frame::IDFrame<IDType, DataT>>;

			using containerT = detail::SyncMapConnector<IDType, FrameDataT>;

			containerT storage;

		public:

			SyncMapConnector() : IConnector<FrameDataT>(), storage(queueCapacity) {}

			virtual bool push(const FrameDataT& data) override {
				return storage.insert(std::pair{ data->getID(), data }).second;
			}

			virtual bool hasValue() override {
				return storage.hasValue();
			}

			virtual std::size_t size() override {
				return storage.size();
			}

			virtual bool extract(FrameDataT& data) override {
				return storage.extract(data);
			}
		};

		template <typename IDType, typename DataT, size_t queueCapacity>
		class SyncMapConnector<Frame::IDFrame<IDType, DataT>, queueCapacity> : public IConnector<Frame::IDFrame<IDType, DataT>> {

			using FrameDataT = Frame::IDFrame<IDType, DataT>;

			using containerT = detail::SyncMapConnector<IDType, FrameDataT>;

			containerT storage;

		public:

			SyncMapConnector() : IConnector<FrameDataT>(), storage(queueCapacity) {}

			virtual bool push(const FrameDataT& data) override {
				return storage.insert(std::pair{ data.getID(), data }).second;
			}

			virtual bool hasValue() override {
				return storage.hasValue();
			}

			virtual std::size_t size() override {
				return storage.size();
			}

			virtual bool extract(FrameDataT& data) override {
				return storage.extract(data);
			}
		};
	}
}