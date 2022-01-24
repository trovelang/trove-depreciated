#pragma once

#include <vector>
#include <types.h>
#include <token.h>

namespace trove {

	struct Type {

		enum class BaseType : u8 {
			INCOMPLETE,
			NONE,
			U32,
			S32,
			STRING,
			STRUCT,
			TYPE,
			FN
		};

		enum class Mutability : u8 {
			MUT,
			CONST
		};

		struct FnType {
			u1 is_lambda = false;
		};

		struct StructType {
			u1 is_anonymous = false;
		};

		u1 equals(Type other);
		std::string to_string();
		void coerce(Type& other);

		u1 complete = true;  // 'var u32' is complete, 'var' alone is not
		BaseType base_type;
		FnType fn_type;
		StructType struct_type;
		Type* contained_type;
		std::vector<Type> contained_types;
		Token* associated_token;
		Mutability mutability{ Mutability::CONST };
	};

	class TypeBuilder {
	public:
		static TypeBuilder builder() {
			return {};
		}

		Type build() {
			return m_internal_type;
		}

		TypeBuilder& complete(u1 complete) {
			m_internal_type.complete = complete;
			return *this;
		}

		TypeBuilder& base_type(Type::BaseType type) {
			m_internal_type.base_type = type;
			return *this;
		}

		TypeBuilder& lambda(u1 lambda) {
			m_internal_type.fn_type.is_lambda = lambda;
			return *this;
		}

		TypeBuilder& anonymous(u1 anonymous) {
			m_internal_type.struct_type.is_anonymous = true;
			return *this;
		}

		TypeBuilder& contained_type(Type* contained_type) {
			m_internal_type.contained_type = contained_type;
			return *this;
		}

		TypeBuilder& contained_types(std::vector<Type> contained_types) {
			m_internal_type.contained_types = contained_types;
			return *this;
		}

		TypeBuilder& associated_token(Token* associated_token) {
			m_internal_type.associated_token = associated_token;
			return *this;
		}

		TypeBuilder& mutability(Type::Mutability mutability) {
			m_internal_type.mutability = mutability;
			return *this;
		}

	private:
		Type m_internal_type;
	};

	extern const char* type_debug[];

}