#pragma once

#include <log.h>
#include <types.h>
#include <string_view>
#include <string>
#include <position.h>

namespace trove {
	
	struct Token {
		enum class Type : u8 {
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

			BOOL,
			U32,
			S32,
			TYPE,
			STRUCT,

			COMP,

			PUB,
			PRIV,
			VAR,
			CONSTANT,

			TRUEY,
			FALSY
		};

		std::string to_string();

		Type type;
		// FIXME this should be std::optional<std::string>
		std::string value {};
		SourcePosition source_position {};
	};

	class TokenBuilder {
	public:
		static TokenBuilder builder() {
			return {};
		}

		Token build() {
			return m_internal_token;
		}

		TokenBuilder& type(Token::Type type) {
			m_internal_token.type = type;
			return *this;
		}

		TokenBuilder& value(std::string value) {
			m_internal_token.value = value;
			return *this;
		}

		TokenBuilder& position(SourcePosition position) {
			m_internal_token.source_position = position;
			return *this;
		}

	private:
		Token m_internal_token;
	};

	extern const char* token_type_debug[];

}