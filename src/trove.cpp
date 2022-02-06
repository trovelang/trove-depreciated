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
#include <filesystem>

std::string working_dir_from_entry(std::string entry_point){
	const size_t last_slash_idx = entry_point.rfind('/');
	if (std::string::npos != last_slash_idx){
		return entry_point.substr(0, last_slash_idx);
	}
	return "";
}

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

std::string load_file(std::string& file){
	std::ifstream t(file);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

std::string load_file_with_working_dir(std::string& working_dir, std::string& file){
	auto full_path = working_dir.append(file);
	if(std::filesystem::exists(full_path)){
		return load_file(full_path);
	}
	return load_file(file);
}

u32 num_lines(std::string& source) {
	u32 line_counter = 1;
	for (auto& c : source)
		if (c == '\n')
			line_counter++;
	return line_counter;
}

s32 compile(std::string& source_name){

	auto source = load_file(source_name);
	auto n_lines = num_lines(source);

	auto working_dir = "c:/trovelang/trove/tests/trove/";

	auto compilation_unit = trove::CompilationUnit(source_name, source, working_dir);

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

s32 output_tokens(std::string& source_name){

	auto source = load_file(source_name);
	auto n_lines = num_lines(source);

	auto working_dir = "c:/trovelang/trove/tests/trove/";
	auto compilation_unit = trove::CompilationUnit(source_name, source, working_dir);
	auto tokens = compilation_unit.lex();

	for(auto& token : *tokens)
		logger.info() << token.to_string() << "\n";

	return 0;
}

s32 output_ast(std::string& source_name){

	auto source = load_file(source_name);
	auto n_lines = num_lines(source);
	
	auto working_dir = "c:/trovelang/trove/tests/trove/";
	auto compilation_unit = trove::CompilationUnit(source_name, source, working_dir);
	auto pass1_result = compilation_unit.up_to_pass1();
	logger.info() << pass1_result.ast->to_string() << "\n";
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
	// auto f = std::string("c:/trovelang/trove/tests/trove/helloworld.trove");
	// compile(load_file(f));
	// return 0;

	if(argc==1){
		help();
	}else{
		if(std::string(argv[1])=="-h"){
			help();
			return 0;
		}

		auto filename = std::string(argv[2]);
		
		if(std::string(argv[1])=="-c"){
			compile(filename);
		}else if(std::string(argv[1])=="-r"){
			auto filename = std::string(argv[2]);
			compile(filename);
			system("c:/trovelang/trove/tmp/tmp.exe");
		}else if(std::string(argv[1])=="-i"){
			logger.warn() << "This feature is not implemented yet\n";
		}else if(std::string(argv[1])=="-t"){
			auto filename = std::string(argv[2]);
			output_tokens(filename);
		}else if(std::string(argv[1])=="-a"){
			auto filename = std::string(argv[2]);
			output_ast(filename);
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
