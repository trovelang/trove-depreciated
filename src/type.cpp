

#include <type.h>

namespace trove {

	const char* type_debug[] = {
		"INCOMPLETE",
		"NONE",
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

	u1 Type::equals(Type other) {
		return type == other.type;
	}

}