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

	enum ScopeModifier : u8 {
		GLOBAL,
		LOCAL
	};

	enum MutabilityModifier : u8 {
		MUT,
		CONST
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
		ScopeModifier& get_scope_modifier() {
			return scope_modifier;
		}
		MutabilityModifier& get_mutability_modifier() {
			return mutability_modifier;
		}
		std::string to_string();

		u1 equals(Type other);
	private:
		std::vector<Type> multiple;
		TypeType type;
		Type* contained;
		Token* token; // this can be for structs, and lambdas
		ScopeModifier scope_modifier = ScopeModifier::GLOBAL;
		MutabilityModifier mutability_modifier = MutabilityModifier::CONST;
	};

	extern const char* type_debug[];

}