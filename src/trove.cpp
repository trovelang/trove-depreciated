#include <iostream>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <chrono>
#include <lexer.h>
#include <parser.h>
#include <analyse.h>
#include <cgenerator.h>
#include <pass1.h>
#include <borrowchecker.h>
#include <unit.h>
#include <log.h>

std::string time_to_str(long long microseconds){
	std::stringstream ss;
	if(microseconds>1000000){
		ss << ((float)microseconds / (float)1000000.0) << " seconds";
	}else if(microseconds>1000){
		ss << ((float)microseconds / (float)1000.0) << " milliseconds";
	}else{
		ss << microseconds << " microseconds";
	}
	return ss.str();
}

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

	auto start_parse = std::chrono::high_resolution_clock::now();
	compilation_unit.up_to_compile(); 
	auto end_gen = std::chrono::high_resolution_clock::now();

	spdlog::info("Compiled {} lines in {}", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_gen - start_parse).count()));

	return 0;

	/*

	auto start_parse = std::chrono::high_resolution_clock::now();

	auto lexer = trove::Lexer(&compilation_unit, source);
	auto tokens = lexer.lex();

	auto parser = trove::Parser(&compilation_unit, tokens);
	auto ast = parser.parse();

	auto type_checker = trove::Pass1(&compilation_unit, ast);
	type_checker.analyse();

	auto end_parse = std::chrono::high_resolution_clock::now();

	spdlog::info("Parsed {} lines in {}...", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_parse - start_parse).count()));

	auto start_gen = std::chrono::high_resolution_clock::now();
	auto cgenerator = trove::CGenerator(ast);
	cgenerator.gen();
	auto end_gen = std::chrono::high_resolution_clock::now();

	spdlog::info("Generated {} lines in {}...", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_gen - start_gen).count()));
	spdlog::info("Compiled {} lines in {}", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_gen - start_parse).count()));	

	return 0;

	*/
}

s32 output_tokens(std::string source){
	auto n_lines = num_lines(source);

	auto compilation_unit = trove::CompilationUnit(source);
	auto tokens = compilation_unit.lex();

	for(auto& token : *tokens)
		logger.info() << token.to_string() << "\n";

	return 0;
}

s32 output_ast(std::string source){
	auto n_lines = num_lines(source);
	auto compilation_unit = trove::CompilationUnit(source);
	auto ast = compilation_unit.up_to_parse();
	logger.info() << ast->to_string() << "\n";
	return 0;
}

void help(){
	logger.info() << "usage: TODO\n";
	logger.info() << "-h Help\n";
	logger.info() << "-c Compile File\n";
	logger.info() << "-r Compile & Run\n";
	logger.info() << "-i Interactive REPL\n";
	logger.info() << "-t Output Tokens\n";
	logger.info() << "-a Output AST\n";
}

s32 args_parser(int argc, char** argv){
	//compile(load_file("c:/trovelang/trove/tests/trove/helloworld.trove"));
	//return 0;
	if(argc==1){
		help();
	}else{
		if(std::string(argv[1])=="-h"){
			help();
		}
		else if(std::string(argv[1])=="-c"){
			compile(load_file(argv[2]));
		}else if(std::string(argv[1])=="-r"){
			compile(load_file(argv[2]));
			system("c:/trovelang/trove/tmp/tmp.exe");
		}else if(std::string(argv[1])=="-i"){
			logger.warn() << "This feature is not implemented yet\n";
		}else if(std::string(argv[1])=="-t"){
			output_tokens(load_file(argv[2]));
		}else if(std::string(argv[1])=="-a"){
			output_ast(load_file(argv[2]));
		}
	}
	return 0;
}

s32 main(int argc, char** argv){
	#ifdef DEBUG
		logger.info() << "running in debug mode!\n";
		logger.warn() << "running in debug mode!\n";
		logger.errr() << "running in debug mode!\n";
	#endif
	args_parser(argc, argv);
	return 0;
}
