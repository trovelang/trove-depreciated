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

std::optional<std::string> load_file_with_working_dir(std::string working_dir, std::string& file){
	auto full_path = working_dir.append(file);
	if(std::filesystem::exists(full_path)){
		return load_file(full_path);
	}else if(std::filesystem::exists(file)){
		return load_file(file);
	}
	return std::optional<std::string>();
}

std::optional<std::string> get_working_dir(std::string current_path, std::string& file_name){
	auto filename_stripped = file_name.substr(0, file_name.find_last_of("\\/")+1);
	auto full_path = current_path.append(filename_stripped);
	if(std::filesystem::exists(full_path)){
		return full_path;
	}else if(std::filesystem::exists(file_name)){
		return filename_stripped;
	}
	return std::optional<std::string>();
}

u32 num_lines(std::string& source) {
	u32 line_counter = 1;
	for (auto& c : source)
		if (c == '\n')
			line_counter++;
	return line_counter;
}

s32 compile(std::string& current_path, std::string& source_name){

	auto working_dir = get_working_dir(current_path, source_name);

	if(!working_dir.has_value()){
		logger.errr() << "file doesn't exist "<<source_name<<".\n";
	}

	auto source = load_file_with_working_dir(working_dir.value(), source_name);

	assert(source.has_value());

	auto n_lines = num_lines(source.value());

	auto compilation_unit = trove::CompilationUnit(source_name, source.value(), working_dir.value());

	auto start_parse = std::chrono::high_resolution_clock::now();
	compilation_unit.up_to_compile(); 
	auto end_gen = std::chrono::high_resolution_clock::now();

	spdlog::info("Compiled {} lines in {}", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_gen - start_parse).count()));

	return 0;
}

s32 output_tokens(std::string& current_path, std::string& source_name){

	auto working_dir = get_working_dir(current_path, source_name);

	if(!working_dir.has_value()){
		logger.errr() << "file doesn't exist "<<source_name<<".\n";
	}

	auto source = load_file_with_working_dir(working_dir.value(), source_name);

	assert(source.has_value());

	auto n_lines = num_lines(source.value());

	auto compilation_unit = trove::CompilationUnit(source_name, source.value(), working_dir.value());
	auto tokens = compilation_unit.lex();

	for(auto& token : *tokens)
		logger.info() << token.to_string() << "\n";

	return 0;
}

s32 output_ast(std::string& current_path, std::string& source_name){

	auto working_dir = get_working_dir(current_path, source_name);

	if(!working_dir.has_value()){
		logger.errr() << "file doesn't exist "<<source_name<<".\n";
	}

	auto source = load_file_with_working_dir(working_dir.value(), source_name);

	assert(source.has_value());

	auto n_lines = num_lines(source.value());
	
	auto compilation_unit = trove::CompilationUnit(source_name, source.value(), working_dir.value());
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
	//auto f = std::string("c:/trovelang/trove/tests/trove/main.trove");
	//compile(f);
	//output_ast(f);
	//return 0;

	auto current_path = std::filesystem::current_path().string();

	if(argc==1){
		help();
	}else{
		if(std::string(argv[1])=="-h"){
			help();
			return 0;
		}

		auto filename = std::string(argv[2]);
		
		if(std::string(argv[1])=="-c"){
			compile(current_path, filename);
		}else if(std::string(argv[1])=="-r"){
			auto filename = std::string(argv[2]);
			compile(current_path, filename);
			system("c:/trovelang/trove/tmp/tmp.exe");
		}else if(std::string(argv[1])=="-i"){
			logger.warn() << "This feature is not implemented yet\n";
		}else if(std::string(argv[1])=="-t"){
			auto filename = std::string(argv[2]);
			output_tokens(current_path, filename);
		}else if(std::string(argv[1])=="-a"){
			auto filename = std::string(argv[2]);
			output_ast(current_path, filename);
		}
	}
	return 0;
}

s32 main(int argc, char** argv){
	#ifdef DEBUG
		logger.debug() << "running in debug mode!\n";
		logger.info() << "running in debug mode!\n";
		logger.warn() << "running in debug mode!\n";
		logger.errr() << "running in debug mode!\n";
	#endif
	args_parser(argc, argv);
	return 0;
}
