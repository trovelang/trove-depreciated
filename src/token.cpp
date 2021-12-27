#include <token.h>
#include <sstream>

namespace trove {
	const char* token_type_debug[] = {
		"END",
			"NUM",
			"IDENTIFIER",
			"STRING",

			"HASH",
			"DIRECTIVE",

			"PLUS",
			"MINUS",
			"STAR",
			"DIV",

			"LPAREN",
			"RPAREN",
			"LBRACKET",
			"RBRACKET",
			"LCURLY",
			"RCURLY",

			"SEMI_COLON",
			"POINTER",
			"COMMA",

			"BAND",
			"LAND",
			"BOR",
			"LOR",

			"BANG",
			"NEQ",

			"ASSIGN",
			"EQUALS",

			"COLON",
			"QUICK_ASSIGN",

			"DOT",
			"DOUBLE_DOT",
			"TRIPLE_DOT",

			"GREATER",
			"GREATER_EQ",

			"LESS",
			"LESS_EQ",

			"RSHIFT",
			"LSHIFT",

			"FN",
			"FOR",
			"LOOP",
			"IF",
			"ELSE",
			"RET",

			"U32",
			"S32",
			"TYPE",
			"STRUCT",

			"COMP",

			"PUB",
			"PRIV",
			"VAR",
			"CONST"
	};

	std::string Token::to_string() {
		std::stringstream ss;
		ss << token_type_debug[type];
		return ss.str();
	}

}