#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>

#include <lexer.h>
#include <parser.h>
#include <analyse.h>
#include <cgenerator.h>

int main(int argc, char** argv)
{

	using namespace trove;

	std::string source = "x s32 = 1+2";

	/*if (argc > 1) {
		std::ifstream t(argv[1]);
		std::stringstream buffer;
		buffer << t.rdbuf();
		source = buffer.str();
	}
	else {
		std::cout << ">";
		std::getline(std::cin, source);
	}*/

	std::ifstream t("C:/trovelang/trove/tests/trove/test.trove");
	std::stringstream buffer;
	buffer << t.rdbuf();
	source = buffer.str();

	auto lexer = Lexer(source);
	auto tokens = lexer.lex();
	for (auto token : tokens) {
		spdlog::info("token {}", token.to_string());
	}

	auto parser = Parser(tokens);
	auto ast = parser.parse();

	std::cout << ast->to_string() << "\n";

	auto analyser = Analyser(ast);
	analyser.analyse();

	auto cgenerator = CGenerator(ast);
	cgenerator.gen();

	// ideally we want to be able to to_string an ast
	return 0;
}
