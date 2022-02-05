#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <chrono>
#include <lexer.h>
#include <parser.h>
#include <analyse.h>
#include <cgenerator.h>
#include <typecheck.h>
#include <borrowchecker.h>
#include <unit.h>

std::string load_file(const char* file){
	std::ifstream t(file);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

u32 num_lines(std::string& source) {
	u32 line_counter = 1;
	for (auto& c : source)
		if (c == '\n')
			line_counter++;
	return line_counter;
}

s32 compile(std::string source){
	auto n_lines = num_lines(source);

	auto compilation_unit = trove::CompilationUnit(source);
	auto err_reporter = trove::ErrorReporter(compilation_unit);


	auto start_parse = std::chrono::high_resolution_clock::now();

	auto lexer = trove::Lexer(err_reporter, source);
	auto tokens = lexer.lex();

	auto parser = trove::Parser(err_reporter, tokens);
	auto ast = parser.parse();

	auto type_checker = trove::TypeCheckPass(err_reporter, ast);
	type_checker.analyse();

	auto borrow_checker = trove::BorrowCheckPass(err_reporter, ast);

	auto end_parse = std::chrono::high_resolution_clock::now();

	spdlog::info("Parsed {} lines in {} microseconds...", n_lines, std::chrono::duration_cast<std::chrono::microseconds>(end_parse - start_parse).count());

	auto start_gen = std::chrono::high_resolution_clock::now();
	auto cgenerator = trove::CGenerator(ast);
	cgenerator.gen();
	auto end_gen = std::chrono::high_resolution_clock::now();

	spdlog::info("Generated {} lines in {} milliseconds...", n_lines, std::chrono::duration_cast<std::chrono::milliseconds>(end_gen - start_gen).count());
	spdlog::info("Compiled {} lines in {} ms", n_lines, std::chrono::duration_cast<std::chrono::milliseconds>(end_gen - start_parse).count());	

	return 0;
}

s32 output_tokens(std::string source){
	auto n_lines = num_lines(source);

	auto compilation_unit = trove::CompilationUnit(source);
	auto err_reporter = trove::ErrorReporter(compilation_unit);


	auto start_parse = std::chrono::high_resolution_clock::now();

	auto lexer = trove::Lexer(err_reporter, source);
	auto tokens = lexer.lex();

	for(auto& token : tokens)
		spdlog::info("{}", token.to_string());

	return 0;
}

s32 output_ast(std::string source){
	auto n_lines = num_lines(source);

	auto compilation_unit = trove::CompilationUnit(source);
	auto err_reporter = trove::ErrorReporter(compilation_unit);


	auto start_parse = std::chrono::high_resolution_clock::now();

	auto lexer = trove::Lexer(err_reporter, source);
	auto tokens = lexer.lex();

	auto parser = trove::Parser(err_reporter, tokens);
	auto ast = parser.parse();

	spdlog::info("{}", ast->to_string());

	return 0;
}

s32 args_parser(int argc, char** argv){

	if(argc==1){
		spdlog::info("usage: TODO");
		spdlog::info("-c Compile File");
		spdlog::info("-r Compile & Run");
		spdlog::info("-i Interactive REPL");
		spdlog::info("-t Output Tokens");
		spdlog::info("-a Output AST");
	}else{
		if(std::string(argv[1])=="-c"){
			spdlog::info("compiling {}", argv[2]);
			compile(load_file(argv[2]));
		}else if(std::string(argv[1])=="-r"){
			spdlog::info("compiling & running {}", argv[2]);
			compile(load_file(argv[2]));
			spdlog::info("running {}", argv[2]);
			system("c:/trovelang/trove/tmp/tmp.exe");
		}else if(std::string(argv[1])=="-i"){
			spdlog::warn("NOT IMPLEMENTED");
		}else if(std::string(argv[1])=="-t"){
			output_tokens(load_file(argv[2]));
		}else if(std::string(argv[1])=="-a"){
			output_ast(load_file(argv[2]));
		}
	}
	return 0;
}

s32 main(int argc, char** argv){
	args_parser(argc, argv);
	return 0;
}
