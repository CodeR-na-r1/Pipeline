#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <memory>

#include "pipeline/frame/IFrame.hpp"
#include "pipeline/frame/Frame.hpp"

#include "pipeline/frame/IIDFrame.hpp"
#include "pipeline/frame/IDFrame.hpp"

BOOST_AUTO_TEST_SUITE(FrameTest)

BOOST_AUTO_TEST_CASE(TestFrame) {

	using namespace Pipeline::Frame;

	using U64 = std::uint64_t;

	using Data_T = std::shared_ptr<U64>;
	using Frame_T = std::shared_ptr<IFrame<Data_T>>;

	auto frame = std::shared_ptr<Frame<Data_T>>{ new Frame<Data_T>{Data_T{new U64{123}}} };

	BOOST_CHECK(*frame->getData() == 123);

	frame.reset(new Frame<Data_T>{ Data_T{new U64{98789}} });
	BOOST_CHECK(*frame->getData() == 98789);

	Data_T newData = Data_T{ new U64{578945} };
	frame.reset(new Frame<Data_T>{ newData });
	BOOST_CHECK(*frame->getData() == 578945);
}

BOOST_AUTO_TEST_CASE(TestIDFrame) {

	using namespace Pipeline::Frame;

	using U64 = std::uint64_t;

	using Data_T = std::shared_ptr<U64>;
	using Frame_T = std::shared_ptr<IFrame<Data_T>>;

	auto frame = std::shared_ptr<IDFrame<U64, Data_T>>{ new IDFrame<U64, Data_T>{13, Data_T{new U64{123}}} };

	BOOST_CHECK(frame->getID() == 13);
	BOOST_CHECK(*frame->getData() == 123);

	frame.reset(new IDFrame<U64, Data_T>{ 12, Data_T{new U64{98789}} });
	BOOST_CHECK(frame->getID() == 12);
	BOOST_CHECK(*frame->getData() == 98789);

	Data_T newData = Data_T{ new U64{578945} };
	frame.reset(new IDFrame<U64, Data_T>{ 8, newData });
	BOOST_CHECK(frame->getID() == 8);
	BOOST_CHECK(*frame->getData() == 578945);
}

BOOST_AUTO_TEST_SUITE_END()