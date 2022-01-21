#pragma once

#include <string>
#include <token.h>
#include <unit.h>
#include <vector>

namespace trove {


	class ErrorReporter {
	public:
		ErrorReporter(CompilationUnit& compilation_unit) : compilation_unit(compilation_unit){
			this->compilation_unit = compilation_unit;
		}
		std::vector<std::string> split_string_by_newline(std::string& str);
		void compile_error(std::string err, SourcePosition position);
		CompilationUnit& compilation_unit;
	};
}