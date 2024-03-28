#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <sstream>

#include "pipeline/stage.hpp"

BOOST_AUTO_TEST_SUITE(StageTest)

BOOST_AUTO_TEST_CASE(TestAddChildForStage) {

	using Pipeline::Stage;

	Stage source = Stage{ std::function([](int data) { return data; }), "source", {} };

	BOOST_CHECK(source.childs.size() == 0);

	Stage child = Stage{ std::function([](int data) { return data; }), "child", {} };
	source.childs.push_back(child);

	BOOST_CHECK(source.childs.size() == 1);
	BOOST_CHECK(source.childs[0].name == "child");
}

BOOST_AUTO_TEST_CASE(TestCallableStage) {

	using Pipeline::Stage;

	Stage source = Stage{ std::function([](int data) { return data * 2; }), "source", {} };

	int data{ 3 };
	BOOST_CHECK(source.callable(data) == data * 2);
}

BOOST_AUTO_TEST_CASE(TestCallableStageForChilds) {

	using Pipeline::Stage;

	Stage source = Stage{ std::function([](int data) { return data * 2; }), "source", {} };
	Stage child = Stage{ std::function([](int data) { return data * 3; }), "child", {} };
	source.childs.push_back(child);

	int data{ 3 };

	BOOST_CHECK(source.callable(data) == data * 2);

	BOOST_CHECK(child.callable(data) == data * 3);
	BOOST_CHECK(source.childs[0].callable(data) == data * 3);
}

BOOST_AUTO_TEST_CASE(TestCreationStagesOfPipeline) {

	using Pipeline::Stage;

	Stage source = Stage{ std::function([](int data) { return data; }), "SOURCE", {} };
	Stage blur = { std::function([](int data) { return data; }), "BLUR", {} };
	Stage convert = { std::function([](int data) { return data; }), "CONVERT", {} };
	Stage binarize = { std::function([](int data) { return data; }), "BINARIZE", {} };
	Stage display = { std::function([](int data) { return data; }), "DISPLAY", {} };

	// in reverse order

	convert.addChild(binarize);
	convert.addChild(display);

	blur.addChild(convert);
	blur.addChild(display);

	source.addChild(blur);
	source.addChild(display);

	std::ostringstream ss{};
	source.dump(ss);

	BOOST_CHECK_EQUAL(ss.view(), "SOURCE(BLUR(CONVERT(BINARIZE(), DISPLAY()), DISPLAY()), DISPLAY())");
}

BOOST_AUTO_TEST_SUITE_END()