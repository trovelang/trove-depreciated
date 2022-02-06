#pragma once

#include <vector>
#include <lexer.h>
#include <parser.h>
#include <error.h>

/*
s32 compile(std::string source){
	auto n_lines = num_lines(source);

	auto compilation_unit = trove::CompilationUnit(source);
	auto err_reporter = trove::ErrorReporter(compilation_unit);


	auto start_parse = std::chrono::high_resolution_clock::now();

	auto lexer = trove::Lexer(err_reporter, source);
	auto tokens = lexer.lex();

	auto parser = trove::Parser(err_reporter, tokens);
	auto ast = parser.parse();

	auto type_checker = trove::Pass1(err_reporter, ast);
	type_checker.analyse();

	auto borrow_checker = trove::BorrowCheckPass(err_reporter, ast);

	auto end_parse = std::chrono::high_resolution_clock::now();

	spdlog::info("Parsed {} lines in {}...", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_parse - start_parse).count()));

	auto start_gen = std::chrono::high_resolution_clock::now();
	auto cgenerator = trove::CGenerator(ast);
	cgenerator.gen();
	auto end_gen = std::chrono::high_resolution_clock::now();

	spdlog::info("Generated {} lines in {}...", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_gen - start_gen).count()));
	spdlog::info("Compiled {} lines in {}", n_lines, time_to_str(std::chrono::duration_cast<std::chrono::microseconds>(end_gen - start_parse).count()));	

	return 0;
}
*/

namespace trove {


	class CompilationUnit {
	public:
		CompilationUnit(std::string source, std::string working_dir) : m_source(source), m_working_dir(working_dir) {
			m_err_reporter = ErrorReporter(this);
		}
		std::vector<Token>* lex();
		AST* parse();
		AST* pass1();
		void compile();
		AST* up_to_parse();
		AST* up_to_pass1();
		void up_to_compile();
		std::string& source(){
			return m_source;
		}
		std::string& working_dir(){
			return m_working_dir;
		}
		ErrorReporter& err_reporter(){
			return m_err_reporter;
		}
	private:
		std::string m_source;
		std::string m_working_dir;
		ErrorReporter m_err_reporter;
		std::vector<Token>* m_tokens;
		AST* m_ast;
	};

}