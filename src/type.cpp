#include <type.h>
#include <sstream>

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
		"FN",
		"MODULE"
	};

	std::string Type::mangle_identifier_with_type(std::string& identifier, Type& type){
        std::stringstream ss;
        ss << identifier << "_" << type.mangled_to_string();
        return ss.str();
    }

	std::string Type::mangle_identifier_with_types(std::string& identifier, std::vector<Type>& types){
		if(types.size()>0)
			return identifier.append("_").append(Type::mangle_types(types));
		return identifier;
	}

	std::string Type::mangle_types(std::vector<Type>& types){
        std::stringstream ss;
		for(u32 i=0;i<types.size();i++){
			ss << types[i].mangled_to_string();
			if(i<types.size()-1)
				ss << "_";
		}
		return ss.str();
	}

	std::string Type::to_string() {
		return type_debug[(u8)base_type];
	}

	std::string Type::mangled_to_string(){
		std::stringstream ss;
		ss << type_debug[(u8)base_type];
		if(base_type==BaseType::FN){
			// We don't include the return type because that isn't useful for mangling
			for(u32 i=0;i<contained_types.size()-1;i++)
				ss << "_" << contained_types[i].mangled_to_string();
		}
		return ss.str();
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