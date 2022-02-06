#include <error.h>
#include <iostream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <unit.h>
#include <log.h>

namespace trove {


	std::vector<std::string> ErrorReporter::split_string_by_newline(std::string& str){
		auto result = std::vector<std::string>{};
		auto ss = std::stringstream{ str };

		for (std::string line; std::getline(ss, line, '\n');)
			result.push_back(line);

		return result;
	}

	void ErrorReporter::compile_error(std::string err, SourcePosition position) {
		logger.errr() << "compile_error in " << m_compilation_unit->source_name() 
		<< " [" << position.line_start << ":" << position.index_start << "]" 
		<< "\n\n";

		std::stringstream ss;

		auto lines = split_string_by_newline(m_compilation_unit->source());

		for(u32 line=position.line_start;line<=position.line_end;line++){
			for(u32 i=0;i<position.index_end;i++){
				if(i==position.index_start)
					ss << logger.RED;
				ss << lines[line][i];
				if(i==position.index_end)
					ss << logger.END;
			}
		}
		
		logger.errr() << ss.str() << "\n\n";
		ss.str("");
		
		for (u32 i = 0; i < position.index_end; i++) {
			if(i<position.index_start)
				ss << " ";
			else
				ss << "^";
		}
		ss << " ";

		logger.errr() << ss.str() << err << "\n";

		exit(0);
	}
}