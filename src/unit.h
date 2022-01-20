#pragma once

namespace trove {


	class CompilationUnit {
	public:
		CompilationUnit(std::string source) : source(source){}
		std::string source;
	};

}