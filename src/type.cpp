

#include <type.h>

namespace trove {

	const char* type_debug[] = {
		"INCOMPLETE",
		"NONE",
		"BOOL",
		"U32",
		"S32",
		"STRING",
		"STRUCT",
		"TYPE",
		"FN"
	};


	std::string Type::to_string() {
		return type_debug[(u8)base_type];
	}

	u1 Type::equals(Type other) {
		if (base_type != other.base_type)
			return false;
		
		switch (base_type) {
			case BaseType::STRUCT: {
				auto equals = true;
				if (contained_types.size() != other.contained_types.size())
					return false;
				for(u32 i=0;i<contained_types.size();i++)
					if (!contained_types[i].equals(other.contained_types[i])) {
						return false;
				}
				return true;
			}
			default:
				return true;
		}
	}

	void Type::coerce(Type& other) {
		other.mutability = mutability;
	}
}