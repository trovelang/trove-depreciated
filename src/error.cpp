#include <error.h>
#include <iostream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <unit.h>

namespace trove {


	std::vector<std::string> ErrorReporter::split_string_by_newline(std::string& str){
		auto result = std::vector<std::string>{};
		auto ss = std::stringstream{ str };

		for (std::string line; std::getline(ss, line, '\n');)
			result.push_back(line);

		return result;
	}

	void ErrorReporter::compile_error(std::string err, SourcePosition position) {
		std::cout << "compile_error" << std::endl;

		std::stringstream ss;

		auto lines = split_string_by_newline(m_compilation_unit->source());

		for (u32 line = position.line_start; line <= position.line_end; line++) {
			for (u32 i = position.index_start; i < position.index_end; i++) {
				ss << lines[line][i];
			}
		}
		
		std::cout << ss.str() << std::endl;
		ss.str("");
		
		for (u32 i = 0; i < position.index_end - position.index_start; i++) {
			ss << "^";
		}
		std::cout << ss.str();
		ss.str("");

		std::cout << err << std::endl;

		exit(0);
	}
}