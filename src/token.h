#pragma once

#include <log.h>
#include <types.h>
#include <string_view>
#include <string>
#include <position.h>

namespace trove {
	
	class Token {
	public:
		enum Type : u8 {
			END,
			NUM,
			IDENTIFIER,
			STRING,

			HASH,
			DIRECTIVE,

			PLUS,
			MINUS,
			STAR,
			DIV,

			LPAREN,
			RPAREN,
			LBRACKET,
			RBRACKET,
			LCURLY,
			RCURLY,

			SEMI_COLON,
			POINTER,
			COMMA,

			BAND,
			LAND,
			PIPE,
			LOR,

			BANG,
			NEQ,

			ASSIGN,
			EQUALS,

			COLON,
			QUICK_ASSIGN,

			DOT,
			DOUBLE_DOT,
			TRIPLE_DOT,

			GREATER,
			GREATER_EQ,

			LESS,
			LESS_EQ,

			RSHIFT,
			LSHIFT,

			FN,
			FOR,
			LOOP,
			IF,
			ELSE,
			RET,

			U32,
			S32,
			TYPE,
			STRUCT,

			COMP,

			PUB,
			PRIV,
			VAR,
			CONST,

			TRUE,
			FALSE
		};

		Token() {}
		Token(Type type, SourcePosition source_position) : type(type), source_position(source_position) {}
		Token(Type type, SourcePosition source_position, std::string value)
			: type(type), source_position(source_position), value(value) {}

		SourcePosition& get_position() {
			return source_position;
		}
		std::string to_string();

		std::string& get_value() {
			return value;
		}

		s32 as_num() {
			return atoi(value.c_str());
		}

		f32 as_float() {}

		Type& get_type() {
			return type;
		}

	private:
		Type type;
		SourcePosition source_position;
		std::string value;
	};

	extern const char* token_type_debug[];

}