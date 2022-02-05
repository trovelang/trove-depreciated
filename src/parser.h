#pragma once

#include <optional>
#include <vector>
#include <ast.h>
#include <token.h>
#include <type.h>
#include <error.h>

namespace trove {
	
	class CompilationUnit;

	class Parser {
	public:
		Parser(CompilationUnit* compilation_unit, std::vector<Token>* tokens) : m_compilation_unit(compilation_unit), m_tokens(tokens) {}
		AST* parse();
		u1 expect(Token::Type);
		std::optional<Token*> consume(Token::Type);
		Token* next();
		Token* peek(u32 ahead);
	private:
		u1 is_type(Token::Type t);
		Type parse_type();
		AST* parse_stmt_expr();
		AST* parse_comp();
		AST* parse_watchman();
		AST* parse_if();
		AST* parse_loop();
		AST* parse_return();
		AST* parse_expr();
		AST* parse_decl_or_assign();
		AST* parse_decl();
		AST* parse_assign();
		AST* parse_plus_minus();
		AST* parse_mul_div();
		AST* parse_unary();
		AST* parse_struct_access();
		AST* parse_call();
		AST* parse_single();
		AST* parse_fn();
		AST* parse_l_curly_thing();
		AST* parse_struct_def();
		AST* parse_struct_literal();
		AST* parse_module();
		AST* parse_block(Token* l_curly, AST* first_item);
		AST* parse_initialiser_list(Token* l_curly, AST* first_item);
		std::vector<Token>* m_tokens;
		CompilationUnit* m_compilation_unit;
		u32 m_current = 0;
	};

}