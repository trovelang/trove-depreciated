#include <parser.h>
#include <spdlog/spdlog.h>

namespace trove {
	AST* Parser::parse() {

		for (auto token : tokens) {
			spdlog::info("token {}", token.to_string());
		}

		auto block_body = std::vector<AST*>();

		while (tokens[current].get_type() != Token::Type::END) {
			block_body.push_back(parse_stmt());
		}

		spdlog::info("ast size {}", block_body[0]->get_type());

		return new AST(AST::Type::PROGRAM, {}, ProgramAST(block_body));

	}

	std::optional<Token*> Parser::consume(Token::Type type) {
		if (expect(type)) {
			return next();
		}
		return std::optional<Token*>();
	}
	u1 Parser::expect(Token::Type type) {
		return tokens[current].get_type() == type;
	}

	Token* Parser::next() {
		return &tokens[current++];
	}

	Token* Parser::peek(u32 ahead = 0) {
		return &tokens[current + ahead];
	}

	u1 Parser::is_type(Token::Type t) {
		switch (t) {
		case Token::Type::CONST:
		case Token::Type::VAR:
		case Token::Type::U32:
		case Token::Type::S32:
		case Token::Type::TYPE:
		case Token::Type::STRUCT:
		case Token::Type::STRING:
		case Token::Type::IDENTIFIER:
		case Token::Type::FN:
			return true;
		}
		return false;
	}

	std::optional<Type> Parser::parse_type() {
		// todo as this is optional, this should be peek
		auto token = peek();

		auto mutability = MutabilityModifier::CONST;

		if (token->get_type() == Token::Type::CONST) {
			mutability = MutabilityModifier::CONST;
			next();
		}else if (token->get_type() == Token::Type::VAR) {
			mutability = MutabilityModifier::MUT;
			next();
		}


		token = next();



		switch (token->get_type()) {
		case Token::Type::U32: return Type(TypeType::U32, token, mutability);
		case Token::Type::S32: return Type(TypeType::S32, token, mutability);
		case Token::Type::TYPE: return Type(TypeType::TYPE, token, mutability);
		case Token::Type::STRUCT: return Type(TypeType::STRUCT, token, mutability);
		case Token::Type::STRING: return Type(TypeType::STRING, token, mutability);
		case Token::Type::IDENTIFIER: return Type(TypeType::STRUCT, token, mutability);
		case Token::Type::FN: {
			auto params = std::vector<Type>();
			if (consume(Token::Type::LPAREN)) {
				while (!expect(Token::Type::RPAREN)) {
					auto param = parse_type();
					if (param.has_value()) {
						params.push_back(param.value());
						if (expect(Token::Type::RPAREN)) {
							break;
						}
						consume(Token::Type::COMMA);
					}
					else {
						// todo err
					}
				}
				consume(Token::Type::RPAREN);
			}

			return Type(TypeType::FN, token, params, mutability);
		}
		}
		return {};
	}

	AST* Parser::parse_stmt() {
		spdlog::info("parse_stmt");
		AST* child = nullptr;
		switch (peek()->get_type()) {
		case Token::Type::LCURLY:
			child = parse_block();
			break;
		case Token::Type::IF:
			child = parse_if();
			break;
		case Token::Type::LOOP:
			child = parse_loop();
			break;
		case Token::Type::RET:
			child = parse_return();
			break;
		default:
			child = parse_decl_or_assign();
			break;
		}
		return new AST(AST::Type::STATEMENT, child->get_position(), StatementAST(child));
	}

	// todo a comp time thing should be done ahead of expressions aswel
	AST* Parser::parse_comp() {
		return parse_decl_or_assign();
	}

	AST* Parser::parse_watchman() {
		auto higher_precedence = parse_block();

		if (consume(Token::Type::PIPE).has_value()) {
			consume(Token::Type::PIPE);
			auto body = parse_stmt();
			return new AST(AST::Type::WATCHMAN, higher_precedence->get_position().merge(body->get_position()), WatchmanAST(higher_precedence, body));
		}

		return higher_precedence;
	}

	AST* Parser::parse_block() {

		auto stmts = std::vector<AST*>();
		auto lcurly = consume(Token::Type::LCURLY);
		while (!expect(Token::Type::RCURLY)) {
			stmts.push_back(parse_stmt());
		}
		auto rcurly = consume(Token::Type::RCURLY);
		return new AST(AST::Type::BLOCK, lcurly.value()->get_position().merge(rcurly.value()->get_position()), BlockAST(stmts));

	}

	AST* Parser::parse_if() {
		auto if_token = consume(Token::Type::IF);
		auto cond = parse_expr();
		auto body = parse_stmt();
		auto else_token = consume(Token::Type::ELSE);
		if (else_token.has_value()) {
			auto else_body = parse_stmt();
			return new AST(AST::Type::IF, if_token.value()->get_position().merge(else_body->get_position()), IfAST(cond, body, else_body));
		}
		return new AST(AST::Type::IF, if_token.value()->get_position().merge(body->get_position()), IfAST(cond, body));
	}

	AST* Parser::parse_loop() {
		auto loop_token = consume(Token::Type::LOOP);

		spdlog::info("parsing loop {}!", peek()->get_value());

		auto cond = parse_expr();
		auto body = parse_stmt();
		return new AST(AST::Type::LOOP, loop_token.value()->get_position().merge(body->get_position()), LoopAST(LoopAST::LoopType::BASIC, cond, body));
	}

	AST* Parser::parse_return() {
		auto return_token = consume(Token::Type::RET);
		auto value = parse_expr();
		return new AST(AST::Type::RET, return_token.value()->get_position().merge(value->get_position()), RetAST(value));
	}

	AST* Parser::parse_expr() {
		return parse_comp();
	}

	AST* Parser::parse_decl_or_assign() {
		auto first = peek();
		auto second = peek(1);
		spdlog::info("parse_decl_or_assign first: {} second: {}", first->get_value(), second->get_value());
		// FIXME: This is probably bad...
		if (first->get_type()==Token::Type::IDENTIFIER && is_type(second->get_type())) {
			spdlog::info("doing decl!");
			return parse_decl();
		}
		spdlog::info("doing assign...");
		return parse_assign();
	}

	AST* Parser::parse_decl() {
		auto higher_precedence = parse_plus_minus();

		auto type = parse_type();

		spdlog::info("PARSING DECL {} {}", higher_precedence->to_string(), type.value().to_string());

		if (consume(Token::Type::ASSIGN)) {
			auto value = parse_expr();

			spdlog::info("type value is {}", type.value().to_string());
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(value->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type, value));
		}
		else {
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(type.value().get_token()->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type));
		}

	
		return higher_precedence;
	}

	AST* Parser::parse_assign() {
		spdlog::info("parsing_assign");
		auto higher_precedence = parse_plus_minus();
		if (expect(Token::Type::ASSIGN)) {
			auto assign_token = consume(Token::Type::ASSIGN);
			auto value = parse_expr();
			spdlog::info("assign lhs pos {} {} {} {}",
				higher_precedence->get_position().index_start,
				higher_precedence->get_position().index_end,
				higher_precedence->get_position().line_start,
				higher_precedence->get_position().line_end);

			spdlog::info("assign rhs pos {} {} {} {}",
				value->get_position().index_start,
				value->get_position().index_end,
				value->get_position().line_start,
				value->get_position().line_end);
			return new AST(AST::Type::ASSIGN, higher_precedence->get_position().merge(value->get_position()), 
				AssignAST(higher_precedence, value));
		}
		return higher_precedence;
	}

	AST* Parser::parse_plus_minus() {
		auto higher_precedence = parse_mul_div();
		if (expect(Token::Type::PLUS) || expect(Token::Type::MINUS)) {
			spdlog::info("parse_plus_minus");
			auto op = next();
			auto rhs = parse_plus_minus();

			BinAST::Type type;
			switch (op->get_type()) {
			case Token::Type::PLUS: type = BinAST::Type::ADD; break;
			case Token::Type::MINUS: type = BinAST::Type::SUB; break;
			}

			return new AST(
				AST::Type::BIN,
				higher_precedence->get_position().merge(rhs->get_position()),
				BinAST(higher_precedence, type, rhs));
		}
		return higher_precedence;
	}

	AST* Parser::parse_mul_div() {
		auto higher_precedence = parse_struct_access();
		if (expect(Token::Type::STAR) || expect(Token::Type::DIV)) {
			auto op = next();
			auto rhs = parse_mul_div();

			BinAST::Type type;
			switch (op->get_type()) {
			case Token::Type::STAR: type = BinAST::Type::MUL; break;
			case Token::Type::DIV: type = BinAST::Type::DIV; break;
			}

			return new AST(
				AST::Type::BIN,
				higher_precedence->get_position().merge(rhs->get_position()),
				BinAST(higher_precedence, type, rhs));
		}
		return higher_precedence;
	}


	AST* Parser::parse_struct_access() {
		auto higher_precedence = parse_call();
		if (consume(Token::Type::DOT)) {
			auto member = next();
			return new AST(AST::Type::STRUCT_ACCESS, higher_precedence->get_position().merge(member->get_position()), StructAccessAST(higher_precedence, member));
		}
		return higher_precedence;
	}

	AST* Parser::parse_call() {
		auto higher_precedence = parse_single();
		if (consume(Token::Type::LPAREN)) {

			auto args = std::vector<AST*>();
			while (!expect(Token::Type::RPAREN)) {
				args.push_back(parse_expr());
				if (expect(Token::Type::RPAREN)) {
					break;
				}
				else {
					consume(Token::Type::COMMA);
				}
			}

			auto right_paren = next();

			return new AST(
				AST::Type::CALL,
				higher_precedence->get_position().merge(right_paren->get_position()),
				CallAST(higher_precedence, args));
		}
		return higher_precedence;
	}


	AST* Parser::parse_single() {
		auto tok = peek();
		switch (tok->get_type()) {
		case Token::Type::NUM: next(); return new AST(AST::Type::NUM, tok->get_position(), NumAST(tok, Type(TypeType::U32)));
		case Token::Type::IDENTIFIER: next(); return new AST(AST::Type::VAR, tok->get_position(), VarAST(tok));
		case Token::Type::STRING: next(); return new AST(AST::Type::STRING, tok->get_position(), StringAST(tok));
		case Token::Type::TRUE: next(); return new AST(AST::Type::BOOL, tok->get_position(), BoolAST(tok));
		case Token::Type::FALSE: next(); return new AST(AST::Type::BOOL, tok->get_position(), BoolAST(tok));
		case Token::Type::FN: return parse_fn();
		// TODO WE WANT TO BE ABLE TO PARSE BLOCKS, OR STRUCT LITERALS without the prepending
		case Token::Type::TYPE: return parse_struct_def();
		case Token::Type::STRUCT: return parse_struct_literal();
		}
		return 0;
	}

	AST* Parser::parse_fn() {

		spdlog::info("parse_fn");

		auto fn = next();

		auto params = std::vector<AST*>();
		if (consume(Token::Type::LPAREN)) {
			while (!expect(Token::Type::RPAREN)) {
				params.push_back(parse_decl());
				if (expect(Token::Type::RPAREN)) {
					break;
				}
				consume(Token::Type::COMMA);
			}
			consume(Token::Type::RPAREN);
		}

		auto body = parse_stmt();

		return new AST(AST::Type::FN, fn->get_position().merge(body->get_position()), FnAST(body, params, 0, Type(TypeType::FN)));

	}

	AST* Parser::parse_struct_def() {

		auto t = consume(Token::Type::TYPE);
		auto left_curly = consume(Token::Type::LCURLY);

		std::vector<AST*> member_decls;
		while (!expect(Token::Type::RCURLY)) {
			member_decls.push_back(parse_decl());
		}

		auto right_curly = consume(Token::Type::RCURLY);

		return new AST(AST::Type::STRUCT_DEF, t.value()->get_position().merge(right_curly.value()->get_position()), StructDefAST(member_decls));
	}

	AST* Parser::parse_struct_literal() {
		auto s = consume(Token::Type::STRUCT);
		auto left_curly = consume(Token::Type::LCURLY);

		std::vector<AST*> member_values;
		while (!expect(Token::Type::RCURLY)) {
			member_values.push_back(parse_expr());
		}

		auto right_curly = consume(Token::Type::RCURLY);

		return new AST(AST::Type::STRUCT_LITERAL, s.value()->get_position().merge(right_curly.value()->get_position()), StructLiteralAST(member_values));
	}

}