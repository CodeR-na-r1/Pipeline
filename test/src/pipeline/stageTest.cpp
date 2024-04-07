#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <sstream>

#include "pipeline/Stage.hpp"

BOOST_AUTO_TEST_SUITE(StageTest)

BOOST_AUTO_TEST_CASE(TestAddChildForStage) {

	using Pipeline::Stage::Stage;

	Stage source = { std::function([](int data) { return data; }), "source", {} };
	BOOST_CHECK(source.getId() == 0);
	BOOST_CHECK(source.getChilds().size() == 0);

	Stage child = { std::function([](int data) { return data; }), "child", {} };
	BOOST_CHECK(child.getId() == 1);
	BOOST_CHECK(child.getChilds().size() == 0);

	source.addChild(child);
	BOOST_CHECK(source.getChilds().size() == 1);
	BOOST_CHECK(source.getId() == 0);
	BOOST_CHECK(child.getId() == 1);
	BOOST_CHECK(source.getChilds()[0].getId() == 2);

	source.addChild(std::move(child));
	BOOST_CHECK(source.getChilds().size() == 2);
	BOOST_CHECK(source.getId() == 0);
	BOOST_CHECK(source.getChilds()[0].getId() == 2);
	BOOST_CHECK(source.getChilds()[1].getId() == 1);

	BOOST_CHECK(source.getChilds()[0].getName() == "child");
	BOOST_CHECK(source.getChilds()[1].getName() == "child");
}

BOOST_AUTO_TEST_CASE(TestCallableStage) {

	using Pipeline::Stage::Stage;

	Stage source = Stage{ std::function([](int data) { return data * 2; }), "source", {} };

	int data{ 3 };
	BOOST_CHECK(source.getCallable()(data) == data * 2);
}

BOOST_AUTO_TEST_CASE(TestCallableStageForChilds) {

	using Pipeline::Stage::Stage;

	Stage source = Stage{ std::function([](int data) { return data * 2; }), "source", {} };
	Stage child = Stage{ std::function([](int data) { return data * 3; }), "child", {} };
	source.getChilds().push_back(child);

	int data{ 3 };

	BOOST_CHECK(source.getCallable()(data) == data * 2);

	BOOST_CHECK(child.getCallable()(data) == data * 3);
	BOOST_CHECK(source.getChilds()[0].getCallable()(data) == data * 3);
}

BOOST_AUTO_TEST_CASE(TestCreationStagesOfPipeline) {

	using Pipeline::Stage::Stage;

	Stage source = Stage{ std::function([](int data) { return data; }), "SOURCE", {} };
	Stage blur = { std::function([](int data) { return data; }), "BLUR", {} };
	Stage convert = { std::function([](int data) { return data; }), "CONVERT", {} };
	Stage binarize = { std::function([](int data) { return data; }), "BINARIZE", {} };
	Stage display = { std::function([](int data) { return data; }), "DISPLAY", {} };

	// in reverse order
	convert.addChild(std::move(binarize));
	convert.addChild(display);

	blur.addChild(std::move(convert));
	blur.addChild(display);

	source.addChild(std::move(blur));
	source.addChild(std::move(display));

	std::ostringstream ss{};
	source.dump(ss);

	BOOST_CHECK_EQUAL(ss.view(), "SOURCE(BLUR(CONVERT(BINARIZE(), DISPLAY()), DISPLAY()), DISPLAY())");
}

BOOST_AUTO_TEST_SUITE_END()