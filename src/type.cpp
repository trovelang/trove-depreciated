

#include <type.h>

namespace trove {

	const char* type_debug[] = {
		"U32",
		"S32",
		"STRING",
		"STRUCT",
		"TYPE",
		"FN"
	};

	std::string Type::to_string() {
		return type_debug[type];
	}


}