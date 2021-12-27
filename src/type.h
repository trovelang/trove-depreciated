#pragma once

#include <vector>
#include <types.h>
#include <token.h>

namespace trove {
	enum TypeType : u8 {
		U32,
		S32,
		STRING,
		STRUCT,
		TYPE,
		FN
	};

	class Type {
	public:
		Type() {}
		Type(TypeType type) : type(type) {}
		Type(TypeType type, Token* token) : type(type), token(token) {}
		Type(TypeType type, std::vector<Type> multiple) : type(type), multiple(multiple) {}
		Type(TypeType type, Token* token, std::vector<Type> multiple) : type(type), token(token), multiple(multiple) {}
		TypeType& get_type() {
			return type;
		}
		Token*& get_token() {
			return token;
		}
		std::vector<Type>& get_multiple() {
			return multiple;
		}
		std::string to_string();
	private:
		std::vector<Type> multiple;
		TypeType type;
		Type* contained;
		Token* token;
	};

	extern const char* type_debug[];

}