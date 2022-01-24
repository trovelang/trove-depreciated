#pragma once

#include <string>
#include <token.h>
#include <unit.h>
#include <vector>

namespace trove {

	class ErrorReporter {
	public:
		ErrorReporter(CompilationUnit& compilation_unit) : m_compilation_unit(compilation_unit){}
		void compile_error(std::string err, SourcePosition position);
	private:
		std::vector<std::string> split_string_by_newline(std::string& str);
		CompilationUnit& m_compilation_unit;
	};
}