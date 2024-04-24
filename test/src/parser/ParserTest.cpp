#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <stdexcept>
#include <fstream>
#include <sstream>
#include <functional>

#include "pipeline/Stage.hpp"
#include "detail/chooser/Chooser.hpp"
#include "parser/Parser.hpp"

BOOST_AUTO_TEST_SUITE(ParserTest)

using namespace Pipeline::detail;

BOOST_AUTO_TEST_CASE(TestCreatePipelineByConfigFromFileWithError) {

	std::ifstream fileJsonConfig("../../test/src/pipeline/PipeConfigWithError.json");

	BOOST_CHECK(fileJsonConfig.is_open() && "File from object ifstream \'fileJsonConfig\' not open!");

	std::shared_ptr<Chooser<int>> chooser{ new Chooser<int>{} };

	chooser->addChoser({ "sourceCallable" , [](int data) {return data; } });

	BOOST_CHECK_EXCEPTION(Pipeline::Parser::JsonParser::fromFile<int>(fileJsonConfig, chooser), std::runtime_error, [](const std::runtime_error& ex) {

		BOOST_CHECK_EQUAL(ex.what(), std::string("Not find callable (function) for stage with name 'gray' <from configuration parser>"));
		return true;
	});

	fileJsonConfig.close();
}

BOOST_AUTO_TEST_CASE(TestCreatePipelineByConfigFromFile) {

	std::ifstream fileJsonConfig("../../test/src/pipeline/PipeConfig.json");

	BOOST_CHECK(fileJsonConfig.is_open() && "File from object ifstream \'fileJsonConfig\' not open!");

	std::shared_ptr<Chooser<int>> chooser{ new Chooser<int>{} };

	chooser->addChoser({ "sourceCallable" , [](int data) {return data; } });
	chooser->addChoser({ "blub" , [](int data) {return data * 3; } });
	chooser->addChoser({ "rgb2gray" , [](int data) {return data; } });

	auto&& source = Pipeline::Parser::JsonParser::fromFile<int>(fileJsonConfig, chooser);

	fileJsonConfig.close();

	std::ostringstream ss{};
	source.dump(ss);

	std::cout << source;

	BOOST_CHECK_EQUAL(ss.view(), "source(gray(error()))");

	BOOST_CHECK(source.getChilds()[0].getChilds()[0].getCallable()(13) == 39);
}

BOOST_AUTO_TEST_SUITE_END()