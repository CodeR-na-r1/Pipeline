#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <memory>

#include "connector/IConnector.hpp"

#include "connector/SPSCConnector.hpp"
#include "connector/MPMCConnector.hpp"
#include "connector/SyncMPMCConnector.hpp"

#include "pipeline/frame/IDFrame.hpp"

BOOST_AUTO_TEST_SUITE(ConnectorTest)

using namespace Pipeline::Connector;

BOOST_AUTO_TEST_CASE(TestIConnectorWithoutID) {

	/* without id -> Frame */

	using DataT = std::shared_ptr<int>;

	using SPSCConnector_T = SPSCConnector<DataT, 1024>;
	using MPMCConnector_T = MPMCConnector<DataT, 1024>;

	std::shared_ptr<IConnector<DataT>> iCon0 = std::shared_ptr<SPSCConnector_T>{ new SPSCConnector_T{} };
	std::shared_ptr<IConnector<DataT>> iCon1 = std::shared_ptr<MPMCConnector_T>{ new MPMCConnector_T{} };
}

BOOST_AUTO_TEST_CASE(TestIConnectorWithID) {

	/* with id -> IDFrame */

	using namespace Pipeline::Frame;

	using U64 = std::uint64_t;
	using DataT = std::shared_ptr<U64>;
	using FrameT = std::shared_ptr<IDFrame<U64, DataT>>;

	using SPSCConnector_T = SPSCConnector<FrameT, 1024>;
	using MPMCConnector_T = MPMCConnector<FrameT, 1024>;
	using SyncConnector_T = SyncMapConnector<FrameT, 1024>;

	std::shared_ptr<IConnector<FrameT>> iCon0 = std::shared_ptr<SPSCConnector_T>{ new SPSCConnector_T{} };
	std::shared_ptr<IConnector<FrameT>> iCon1 = std::shared_ptr<MPMCConnector_T>{ new MPMCConnector_T{} };
	std::shared_ptr<IConnector<FrameT>> iCon2 = std::shared_ptr<SyncConnector_T>{ new SyncConnector_T{} };
}

BOOST_AUTO_TEST_CASE(TestSPSCConnector) {

	using DataT = std::shared_ptr<int>;

	SPSCConnector<DataT, 1024> spscConnector;

	BOOST_CHECK(spscConnector.hasValue() == false);

	spscConnector.push(DataT{ new int{1} });
	BOOST_CHECK(spscConnector.hasValue() == true);

	spscConnector.push(DataT{ new int{2} });
	BOOST_CHECK(spscConnector.hasValue() == true);

	spscConnector.push(DataT{ new int{3} });
	BOOST_CHECK(spscConnector.hasValue() == true);

	DataT value{};
	spscConnector.extract(value);
	BOOST_CHECK(*value == 1);

	spscConnector.extract(value);
	BOOST_CHECK(*value == 2);

	spscConnector.extract(value);
	BOOST_CHECK(*value == 3);

	BOOST_CHECK(spscConnector.hasValue() == false);

	spscConnector.push(DataT{ new int{123} });
	BOOST_CHECK(spscConnector.hasValue() == true);

	spscConnector.extract(value);
	BOOST_CHECK(*value == 123);
}

BOOST_AUTO_TEST_CASE(TestMPMCConnector) {

	using DataT = std::shared_ptr<int>;

	MPMCConnector<DataT, 1024> mpmcConnector;

	BOOST_CHECK(mpmcConnector.hasValue() == false);

	mpmcConnector.push(DataT{ new int{1} });
	BOOST_CHECK(mpmcConnector.hasValue() == true);

	mpmcConnector.push(DataT{ new int{2} });
	BOOST_CHECK(mpmcConnector.hasValue() == true);

	mpmcConnector.push(DataT{ new int{3} });
	BOOST_CHECK(mpmcConnector.hasValue() == true);

	DataT value{};
	mpmcConnector.extract(value);
	BOOST_CHECK(*value == 1);

	mpmcConnector.extract(value);
	BOOST_CHECK(*value == 2);

	mpmcConnector.extract(value);
	BOOST_CHECK(*value == 3);

	BOOST_CHECK(mpmcConnector.hasValue() == false);

	mpmcConnector.push(DataT{ new int{123} });
	BOOST_CHECK(mpmcConnector.hasValue() == true);

	mpmcConnector.extract(value);
	BOOST_CHECK(*value == 123);
}

BOOST_AUTO_TEST_CASE(TestSyncConnector) {

	using namespace Pipeline::Frame;

	using U64 = std::uint64_t;
	using DataT = std::shared_ptr<U64>;

	using FrameT = std::shared_ptr<IDFrame<U64, DataT>>;
	using SyncConnectorT = Pipeline::Connector::SyncMapConnector<FrameT, 1024>;

	SyncConnectorT syncConnector{};

	BOOST_CHECK(syncConnector.hasValue() == false);

	syncConnector.push(FrameT{ new IDFrame<U64, DataT>{2, DataT{new U64{200}}} });
	BOOST_CHECK(syncConnector.hasValue() == false);

	syncConnector.push(FrameT{ new IDFrame<U64, DataT>{0, DataT{new U64{10}}} });
	BOOST_CHECK(syncConnector.hasValue() == true);

	syncConnector.push(FrameT{ new IDFrame<U64, DataT>{3, DataT{new U64{300}}} });
	BOOST_CHECK(syncConnector.hasValue() == true);

	FrameT value{};
	bool ret = false;
	ret = syncConnector.extract(value);
	BOOST_CHECK(ret == true);
	BOOST_CHECK(value->getID() == 0);
	BOOST_CHECK(*value->getData() == 10);
	BOOST_CHECK(syncConnector.hasValue() == false);

	ret = syncConnector.extract(value);
	BOOST_CHECK(ret == false);
	BOOST_CHECK(syncConnector.hasValue() == false);

	syncConnector.push(FrameT{ new IDFrame<U64, DataT>{1, DataT{new U64{100}}} });
	BOOST_CHECK(syncConnector.hasValue() == true);

	ret = syncConnector.extract(value);
	BOOST_CHECK(ret == true);
	BOOST_CHECK(value->getID() == 1);
	BOOST_CHECK(*value->getData() == 100);
	BOOST_CHECK(syncConnector.hasValue() == true);

	ret = syncConnector.extract(value);
	BOOST_CHECK(ret == true);
	BOOST_CHECK(value->getID() == 2);
	BOOST_CHECK(*value->getData() == 200);
	BOOST_CHECK(syncConnector.hasValue() == true);

	ret = syncConnector.extract(value);
	BOOST_CHECK(ret == true);
	BOOST_CHECK(value->getID() == 3);
	BOOST_CHECK(*value->getData() == 300);
	BOOST_CHECK(syncConnector.hasValue() == false);
	
	ret = syncConnector.extract(value);
	BOOST_CHECK(ret == false);
	BOOST_CHECK(syncConnector.hasValue() == false);
}

BOOST_AUTO_TEST_SUITE_END()