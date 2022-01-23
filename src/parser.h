#pragma once

#include <optional>
#include <vector>
#include <ast.h>
#include <token.h>
#include <type.h>
#include <error.h>

namespace trove {
	class Parser {
	public:
		Parser(ErrorReporter err_reporter, std::vector<Token> tokens) : err_reporter(err_reporter), tokens(tokens) {}
		AST* parse();
		u1 expect(Token::Type);
		std::optional<Token*> consume(Token::Type);
		Token* next();
		Token* peek(u32 ahead);
	private:
		ErrorReporter err_reporter;
		u32 current = 0;
		u1 is_type(Token::Type t);
		Type parse_type();
		AST* parse_stmt();
		AST* parse_comp();
		AST* parse_watchman();
		AST* parse_block();
		AST* parse_if();
		AST* parse_loop();
		AST* parse_return();
		AST* parse_expr();
		AST* parse_decl_or_assign();
		AST* parse_decl();
		AST* parse_assign();
		AST* parse_plus_minus();
		AST* parse_mul_div();
		AST* parse_struct_access();
		AST* parse_call();
		AST* parse_single();
		AST* parse_fn();
		AST* parse_struct_def();
		AST* parse_struct_literal();
		std::vector<Token> tokens;
	};

}