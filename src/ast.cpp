#include <ast.h>

namespace trove {

	const char* ast_type_debug[] = {
		"AST_PROGRAM",
		"AST_BLOCK",
		"AST_STRUCT_DEF",
		"AST_STRUCT_LITERAL",
		"AST_STRUCT_ACCESS",
		"AST_FN",
		"AST_NUM",
		"AST_VAR",
		"AST_STRING",
		"AST_CALL",
		"AST_BIN",
		"AST_DECL",
		"AST_ASSIGN",
		"AST_IF",
		"AST_RET",
		"AST_LOOP"
	};

}