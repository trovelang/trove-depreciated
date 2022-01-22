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

	struct Type {
		Type() {}
		Type(TypeType type) : type(type) {}
		Type(TypeType type, Token* token, MutabilityModifier mutability_modifier) 
			: type(type), token(token), mutability_modifier(mutability_modifier) {}
		Type(TypeType type, std::vector<Type> multiple) : type(type), multiple(multiple) {}
		Type(TypeType type, Token* token, std::vector<Type> multiple) : type(type), token(token), multiple(multiple) {}
		Type(TypeType type, Token* token, std::vector<Type> multiple, MutabilityModifier mutability_modifier)
			: type(type), token(token), multiple(multiple), mutability_modifier(mutability_modifier) {}
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
		u1 is_lambda;
		std::vector<Type> multiple;
		TypeType type;
		Type* contained;
		Token* token; // this can be for structs, and lambdas
		ScopeModifier scope_modifier = ScopeModifier::GLOBAL;
		MutabilityModifier mutability_modifier = MutabilityModifier::CONST;
	};

	extern const char* type_debug[];

}