#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

#include <fstream>
#include <sstream>
#include <functional>

#include "pipeline/stage.hpp"
#include "parser/parser.hpp"

BOOST_AUTO_TEST_SUITE(ParserTest)

BOOST_AUTO_TEST_CASE(TestCreatePipelineByConfigFromFileWithError) {

	std::ifstream fileJsonConfig("../../test/src/pipeline/pipeConfigWithError.json");

	std::cout << std::filesystem::current_path() << std::endl;

	BOOST_CHECK(fileJsonConfig.is_open() && "File from object ifstream \'fileJsonConfig\' not open!");

	auto source = Pipeline::JsonParser::fromFile<int>(fileJsonConfig, [](std::string callableName) -> std::function<int(int)> {

		if (callableName == "getImage") {
			return [](int data) { return data; };
		}

		return [](int data) { return data; };
	});

	BOOST_CHECK(source.has_value() == false);
}

BOOST_AUTO_TEST_CASE(TestCreatePipelineByConfigFromFile) {

	std::ifstream fileJsonConfig("../../test/src/pipeline/pipeConfig.json");

	BOOST_CHECK(fileJsonConfig.is_open() && "File from object ifstream \'fileJsonConfig\' not open!");

	auto source = Pipeline::JsonParser::fromFile<int>(fileJsonConfig, [](std::string callableName) -> std::function<int(int)> {

		if (callableName == "getImage") {
			return [](int data) { return data; };
		}
		else if (callableName == "rgb2gray") {
			return [](int data) { return data; };
		}
		else if (callableName == "gaussian") {
			return [](int data) { return data; };
		}
		else if (callableName == "display") {
			return [](int data) { return data; };
		}

		return [](int data) { return data; };
		}).value_or(Pipeline::Stage<int>([](int data) {return data; }, {}, {}));

	std::ostringstream ss{};
	source.dump(ss);

	std::cout << source;

	BOOST_CHECK_EQUAL(ss.view(), "source(gray(error()))");
}

BOOST_AUTO_TEST_SUITE_END()