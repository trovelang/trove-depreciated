#include <parser.h>
#include <spdlog/spdlog.h>

namespace trove {
	AST* Parser::parse() {

		spdlog::info("parsing!");

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
		case Token::Type::FN:
			return true;
		}
		return false;
	}

	std::optional<Type> Parser::parse_type() {
		// todo as this is optional, this should be peek
		auto token = peek();

		if (token->get_type() == Token::Type::CONST || token->get_type() == Token::Type::VAR) {
			next();
		}


		token = next();



		switch (token->get_type()) {
		case Token::Type::U32: return Type(TypeType::U32, token);
		case Token::Type::S32: return Type(TypeType::S32, token);
		case Token::Type::TYPE: return Type(TypeType::TYPE, token);
		case Token::Type::STRUCT: return Type(TypeType::STRUCT, token);
		case Token::Type::STRING: return Type(TypeType::STRING, token);
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

			return Type(TypeType::FN, token, params);
		}
		}
		return {};
	}

	AST* Parser::parse_stmt() {
		spdlog::info("parse_stmt");
		switch (peek()->get_type()) {
		case Token::Type::LCURLY:
			return parse_block();
		case Token::Type::IF:
			return parse_if();
		case Token::Type::LOOP:
			return parse_loop();
		case Token::Type::RET:
			return parse_return();
		default:
			return parse_decl_or_assign();
		}
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
		spdlog::info("parse_decl_or_assign");
		auto first = peek();
		auto second = peek(1);
		if (is_type(second->get_type())) {
			return parse_decl();
		}
		return parse_assign();
	}

	AST* Parser::parse_decl() {
		auto higher_precedence = parse_plus_minus();

		auto type = parse_type();
		if (!type.has_value()) {
			type = Type();
		}

		if (consume(Token::Type::ASSIGN)) {
			spdlog::info("initialising decl!");
			auto value = parse_expr();
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(value->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type.value(), value));
		}
		else {
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(type.value().get_token()->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type.value()));
		}

	
		return higher_precedence;
	}

	AST* Parser::parse_assign() {
		spdlog::info("parsing_assign");
		auto higher_precedence = parse_plus_minus();
		if (expect(Token::Type::ASSIGN)) {
			auto assign_token = consume(Token::Type::ASSIGN);
			auto value = parse_expr();
			return new AST(AST::Type::ASSIGN, higher_precedence->get_position().merge(value->get_position()), AssignAST(higher_precedence, value));
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
		case Token::Type::NUM: next(); return new AST(AST::Type::NUM, tok->get_position(), NumAST(tok));
		case Token::Type::IDENTIFIER: next(); return new AST(AST::Type::VAR, tok->get_position(), VarAST(tok));
		case Token::Type::STRING: next(); return new AST(AST::Type::STRING, tok->get_position(), StringAST(tok));
		case Token::Type::FN: return parse_fn();
		case Token::Type::LCURLY: return parse_struct_def();
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

		auto left_curly = consume(Token::Type::LCURLY);

		std::vector<AST*> member_decls;
		while (!expect(Token::Type::RCURLY)) {
			member_decls.push_back(parse_decl());
		}

		auto right_curly = consume(Token::Type::RCURLY);

		return new AST(AST::Type::STRUCT_DEF, left_curly.value()->get_position().merge(right_curly.value()->get_position()), StructDefAST(member_decls));
	}

	AST* Parser::parse_struct_literal() {
		auto left_curly = consume(Token::Type::LCURLY);

		std::vector<AST*> member_values;
		while (!expect(Token::Type::RCURLY)) {
			member_values.push_back(parse_expr());
		}

		auto right_curly = consume(Token::Type::RCURLY);

		return new AST(AST::Type::STRUCT_LITERAL, left_curly.value()->get_position().merge(right_curly.value()->get_position()), StructLiteralAST(member_values));
	}

}