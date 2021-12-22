#pragma once

#include <optional>
#include <vector>
#include <ast.h>
#include <token.h>
#include <type.h>

class Parser {
public:
	Parser() {}
	Parser(std::vector<Token> tokens) : tokens(tokens) {}
	AST* parse();
	u1 expect(Token::Type);
	std::optional<Token*> consume(Token::Type);
	Token* next();
	Token* peek(u32 ahead);
private:
	u32 current = 0;
	Type parse_type();
	AST* parse_stmt();
	AST* parse_comp();
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
