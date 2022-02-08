#include <parser.h>
#include <spdlog/spdlog.h>
#include <trove.h>

namespace trove {
	AST* Parser::parse() {

		auto block_body = std::vector<AST*>();

		while (m_tokens->at(m_current).type != Token::Type::END) {
			block_body.push_back(parse_stmt_expr());
		}

		return new AST(AST::Type::PROGRAM, {}, ProgramAST(block_body));
	}

	std::optional<Token*> Parser::consume(Token::Type type) {
		if (expect(type)) {
			return next();
		}
		return std::optional<Token*>();
	}
	u1 Parser::expect(Token::Type type) {
		return m_tokens->at(m_current).type == type;
	}

	Token* Parser::next() {
		return &m_tokens->at(m_current++);
	}

	Token* Parser::peek(u32 ahead = 0) {
		return &m_tokens->at(m_current + ahead);
	}

	u1 Parser::is_type(Token::Type t) {
		switch (t) {
			case Token::Type::CONSTANT:
			case Token::Type::VAR:
			case Token::Type::BOOL:
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

	Type Parser::parse_type() {

		auto token = peek();

		auto mutability = Type::Mutability::CONSTANT;

		if (token->type == Token::Type::CONSTANT) {
			mutability = Type::Mutability::CONSTANT;
			next();
		}else if (token->type == Token::Type::VAR) {
			mutability = Type::Mutability::MUT;
			next();
		}

		token = peek();

		switch (token->type) {
			case Token::Type::BOOL: next();  return TypeBuilder::builder().base_type(Type::BaseType::BOOL).associated_token(token).mutability(mutability).build();
			case Token::Type::U32: next();  return TypeBuilder::builder().base_type(Type::BaseType::U32).associated_token(token).mutability(mutability).build();
			case Token::Type::S32: next();  return TypeBuilder::builder().base_type(Type::BaseType::S32).associated_token(token).mutability(mutability).build();
			case Token::Type::TYPE: next();  return TypeBuilder::builder().base_type(Type::BaseType::TYPE).associated_token(token).mutability(mutability).build();
			case Token::Type::STRUCT: next();  return TypeBuilder::builder().base_type(Type::BaseType::STRUCT).associated_token(token).mutability(mutability).build();
			case Token::Type::STRING: next();  return TypeBuilder::builder().base_type(Type::BaseType::STRING).associated_token(token).mutability(mutability).build();
			case Token::Type::IDENTIFIER: next(); return TypeBuilder::builder().base_type(Type::BaseType::STRUCT).associated_token(token).mutability(mutability).build();
			case Token::Type::FN: {
				next();
				auto params = std::vector<Type>();
				if (consume(Token::Type::LPAREN)) {
					while (!expect(Token::Type::RPAREN)) {
						auto param = parse_type();
					
						params.push_back(param);
						if (expect(Token::Type::RPAREN)) {
							break;
						}
						consume(Token::Type::COMMA);
		
					}
					consume(Token::Type::RPAREN);
				}

				if (is_type(peek()->type)) {
					params.push_back(parse_type());
				}
				else {
					params.push_back(TypeBuilder::builder().base_type(Type::BaseType::NONE).build());
				}

				return TypeBuilder::builder()
					.base_type(Type::BaseType::FN)
					.associated_token(token)
					.contained_types(params)
					.mutability(mutability)
					.build();
			}
		}
		return TypeBuilder::builder()
			.complete(false)
			.mutability(mutability)
			.build();
	}


	AST* Parser::parse_stmt_expr() {
		auto child = parse_expr();
		return new AST(AST::Type::STATEMENT, child->get_position(), StatementAST(child));
	}

	AST* Parser::parse_annotation(){
		auto annot = consume(Token::Type::DIRECTIVE);
		auto annotation_name = consume(Token::Type::IDENTIFIER);
		auto body = parse_expr();
		return new AST(AST::Type::ANNOTATION, annot.value()->source_position.merge(body->source_position), AnnotationAST(annotation_name.value(), body));
	}

	AST* Parser::parse_expr() {
		AST* child = nullptr;
		auto peeking = peek();
		switch (peek()->type) {
		case Token::Type::DIRECTIVE:
			child = parse_annotation();
			break;
		/*case Token::Type::LCURLY:
			child = parse_block();
			break;*/
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
			child = parse_comp();
			break;
		}
		return child;
	}

	// todo a comp time thing should be done ahead of expressions aswel
	AST* Parser::parse_comp() {
		return parse_decl_or_assign();
	}

	/*AST* Parser::parse_watchman() {
		auto higher_precedence = parse_block();

		IF_VALUE(consume(Token::Type::PIPE)) {
			consume(Token::Type::PIPE);
			auto body = parse_expr();
			return new AST(AST::Type::WATCHMAN, higher_precedence->get_position().merge(body->get_position()), WatchmanAST(higher_precedence, body));
		}

		return higher_precedence;
	}*/

	AST* Parser::parse_if() {
		auto if_token = consume(Token::Type::IF);
		auto cond = parse_expr();
		auto body = parse_expr();
		auto else_token = consume(Token::Type::ELSE);
		IF_VALUE(else_token) {
			auto else_body = parse_expr();
			return new AST(AST::Type::IF, if_token.value()->source_position.merge(else_body->get_position()), IfAST(cond, body, else_body));
		}
		return new AST(AST::Type::IF, if_token.value()->source_position.merge(body->get_position()), IfAST(cond, body));
	}

	AST* Parser::parse_loop() {
		auto loop_token = consume(Token::Type::LOOP);
		auto cond = parse_expr();
		auto body = parse_stmt_expr();
		return new AST(AST::Type::LOOP, loop_token.value()->source_position.merge(body->get_position()), LoopAST(LoopAST::LoopType::BASIC, cond, body));
	}

	AST* Parser::parse_return() {
		auto return_token = consume(Token::Type::RET);
		auto value = parse_expr();
		return new AST(AST::Type::RET, return_token.value()->source_position.merge(value->get_position()), RetAST(value));
	}

	AST* Parser::parse_decl_or_assign() {
		auto first = peek();
		auto second = peek(1);


		if (first->type == Token::Type::IDENTIFIER
			&& (second->type == Token::Type::COLON
				|| second->type == Token::Type::CONSTANT_DECL
				|| second->type == Token::Type::QUICK_ASSIGN)) {

			return parse_decl();
		}

		//if (first->type==Token::Type::IDENTIFIER && is_type(second->type)) {
		//	return parse_decl();
		//}
		return parse_assign();
	}

	AST* Parser::parse_decl() {
		auto higher_precedence = parse_plus_minus();

		if (consume(Token::Type::CONSTANT_DECL)) {
			auto value = parse_expr();
			auto type = TypeBuilder::builder().complete(false).mutability(Type::Mutability::CONSTANT).build();
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(value->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type, value, true));
		}
		else if (consume(Token::Type::QUICK_ASSIGN)) {
			auto value = parse_expr();
			auto type = TypeBuilder::builder().complete(false).mutability(Type::Mutability::MUT).build();
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(value->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type, value, true));
		}
		else if (consume(Token::Type::COLON)) {
			auto type = parse_type();
			auto requires_infering = !type.complete;


			if (consume(Token::Type::ASSIGN)) {
				auto value = parse_expr();
				type.mutability = Type::Mutability::MUT;
				return new AST(
					AST::Type::DECL,
					higher_precedence->get_position().merge(value->get_position()),
					DeclAST(higher_precedence->as_var().get_token(), type, value, false));
			}
			else if (consume(Token::Type::COLON)) {
				auto value = parse_expr();
				type.mutability = Type::Mutability::CONSTANT;
				return new AST(
					AST::Type::DECL,
					higher_precedence->get_position().merge(value->get_position()),
					DeclAST(higher_precedence->as_var().get_token(), type, value, false));
			}
			else {
				type.mutability = Type::Mutability::MUT;
				return new AST(
					AST::Type::DECL,
					higher_precedence->get_position(),
					DeclAST(higher_precedence->as_var().get_token(), type));
			}
		}

		/*

		auto type = parse_type();
		auto requires_infering = !type.complete;


		if (consume(Token::Type::ASSIGN)) {
			auto value = parse_expr();
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(value->get_position()),
				DeclAST(higher_precedence->as_var().get_token(), type, value, requires_infering));
		}
		else {
			// check if we have x var or x const (i.e. we haven't fully quantified the type)
			if (requires_infering) {
				m_error_reporter.compile_error("Type is not fully quantified (you can't just put 'var' or 'const' without an initialiser!)", higher_precedence->source_position);
			}
			return new AST(
				AST::Type::DECL,
				higher_precedence->get_position().merge(type.associated_token->source_position),
				DeclAST(higher_precedence->as_var().get_token(), type));
		}

		*/
		return higher_precedence;
	}

	AST* Parser::parse_assign() {
		auto higher_precedence = parse_plus_minus();
		if (expect(Token::Type::ASSIGN)) {
			auto assign_token = consume(Token::Type::ASSIGN);
			auto value = parse_expr();
			return new AST(AST::Type::ASSIGN, higher_precedence->get_position().merge(value->get_position()), 
				AssignAST(higher_precedence, value));
		}
		return higher_precedence;
	}

	AST* Parser::parse_plus_minus() {
		auto higher_precedence = parse_mul_div();
		if (expect(Token::Type::PLUS) || expect(Token::Type::MINUS)) {
			auto op = next();
			auto rhs = parse_plus_minus();
			BinAST::Type type;
			switch (op->type) {
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
		auto higher_precedence = parse_unary();
		if (expect(Token::Type::STAR) || expect(Token::Type::DIV)) {
			auto op = next();
			auto rhs = parse_mul_div();

			BinAST::Type type;
			switch (op->type) {
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


	AST* Parser::parse_unary() {
		if (expect(Token::Type::BANG) || expect(Token::Type::BAND)) {
			auto op = next();
			auto value = parse_unary();
			auto ast = new AST(AST::Type::UN, op->source_position.merge(value->source_position), UnAST(op, value));
			return ast;

		}
		return parse_call();
	}

	// AST* Parser::parse_struct_access() {
	// 	auto higher_precedence = parse_call();
	// 	if (consume(Token::Type::DOT)) {
	// 		auto member = parse_struct_access();
	// 		return new AST(AST::Type::STRUCT_ACCESS, higher_precedence->get_position().merge(member->source_position), StructAccessAST(higher_precedence, member));
	// 	}
	// 	return higher_precedence;
	// }

	AST* Parser::parse_call() {
		auto higher_precedence = parse_struct_access();
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
				higher_precedence->get_position().merge(right_paren->source_position),
				CallAST(higher_precedence, args));
		}
		return higher_precedence;
	}

	AST* Parser::parse_struct_access() {
		auto higher_precedence = parse_single();
		if (consume(Token::Type::DOT)) {
			auto member = parse_struct_access();
			return new AST(AST::Type::STRUCT_ACCESS, higher_precedence->get_position().merge(member->source_position), StructAccessAST(higher_precedence, member));
		}
		return higher_precedence;
	}

	AST* Parser::parse_single() {
		auto tok = peek();
		switch (tok->type) {
		case Token::Type::NUM: next(); return new AST(AST::Type::NUM, tok->source_position, NumAST(tok, TypeBuilder::builder().base_type(Type::BaseType::U32).build()));
		case Token::Type::IDENTIFIER: next(); return new AST(AST::Type::VAR, tok->source_position, VarAST(tok));
		case Token::Type::STRING: next(); return new AST(AST::Type::STRING, tok->source_position, StringAST(tok));
		case Token::Type::TRUEY: next(); return new AST(AST::Type::BOOL, tok->source_position, BoolAST(BoolAST::Type::TRUEY, TypeBuilder::builder().base_type(Type::BaseType::BOOL).build()));
		case Token::Type::FALSY: next(); return new AST(AST::Type::BOOL, tok->source_position, BoolAST(BoolAST::Type::FALSY, TypeBuilder::builder().base_type(Type::BaseType::BOOL).build()));
		case Token::Type::FN: return parse_fn();
		// TODO WE WANT TO BE ABLE TO PARSE BLOCKS, OR STRUCT LITERALS without the prepending
		case Token::Type::TYPE: return parse_struct_def();
		case Token::Type::STRUCT: return parse_struct_literal();
		case Token::Type::MOD: return parse_module();
		case Token::Type::LCURLY: return parse_l_curly_thing();
		}
		return 0;
	}

	AST* Parser::parse_fn() {

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

		Type return_type = TypeBuilder::builder().base_type(Type::BaseType::NONE).build();
		if (is_type(peek()->type)) {
			return_type = parse_type();
		}

		auto body = parse_stmt_expr();

		auto type = TypeBuilder::builder()
			.base_type(Type::BaseType::FN)
			.build();
		return new AST(AST::Type::FN, fn->source_position.merge(body->get_position()), FnAST(body, params, return_type, 0, type));

	}

	AST* Parser::parse_l_curly_thing() {

		auto l_curly = consume(Token::Type::LCURLY);

		auto first_expression = parse_expr();


		if (first_expression == nullptr) {
			auto r_curly = consume(Token::Type::RCURLY);
			return new AST(AST::Type::BLOCK, l_curly.value()->source_position.merge(r_curly.value()->source_position), BlockAST());
		}

		// we are doing an initialiser list
		if (expect(Token::Type::COMMA)) {
			return parse_initialiser_list(l_curly.value(), first_expression);
		}

		// we are doing a struct def?
		if (first_expression->type == AST::Type::DECL) {

		}

		return parse_block(l_curly.value(), first_expression);
	}

	AST* Parser::parse_struct_def() {

		auto t = consume(Token::Type::TYPE);
		auto left_curly = consume(Token::Type::LCURLY);

		std::vector<AST*> member_decls;
		while (!expect(Token::Type::RCURLY)) {
			member_decls.push_back(parse_decl());
		}

		auto right_curly = consume(Token::Type::RCURLY);

		return new AST(AST::Type::STRUCT_DEF, t.value()->source_position.merge(right_curly.value()->source_position), StructDefAST(member_decls));
	}

	AST* Parser::parse_struct_literal() {
		auto s = consume(Token::Type::STRUCT);
		auto left_curly = consume(Token::Type::LCURLY);

		std::vector<AST*> member_values;
		while (!expect(Token::Type::RCURLY)) {
			member_values.push_back(parse_expr());
		}

		auto right_curly = consume(Token::Type::RCURLY);

		return new AST(AST::Type::STRUCT_LITERAL, s.value()->source_position.merge(right_curly.value()->source_position), StructLiteralAST(member_values));
	}


	AST* Parser::parse_module() {
		auto m = consume(Token::Type::MOD);
		auto left_curly = consume(Token::Type::LCURLY);
		std::vector<AST*> body;
		while (!expect(Token::Type::RCURLY)) {
			body.push_back(parse_stmt_expr());
		}
		auto right_curly = consume(Token::Type::RCURLY); 
		return new AST(AST::Type::MODULE, 
			m.value()->source_position.merge(right_curly.value()->source_position), 
			ModuleAST(body));
	}


	
	AST* Parser::parse_block(Token* l_curly, AST* first_item) {
		auto stmts = std::vector<AST*>();
		stmts.push_back(new AST(AST::Type::STATEMENT, first_item->source_position, StatementAST(first_item)));
		while (!expect(Token::Type::RCURLY)) {
			stmts.push_back(parse_stmt_expr());
		}
		auto r_curly = consume(Token::Type::RCURLY);
		return new AST(AST::Type::BLOCK, l_curly->source_position.merge(r_curly.value()->source_position), BlockAST(stmts));
	}

	/*
	
		
	e.g. x MyType = {1, "2", 3.0}
	
	*/
	AST* Parser::parse_initialiser_list(Token* l_curly, AST* first_item) {

		auto list_items = std::vector<AST*>();
		list_items.push_back(first_item);

		while (!expect(Token::Type::RCURLY)) {
			consume(Token::Type::COMMA);
			auto next_value = parse_expr();
			list_items.push_back(next_value);
		}

		auto r_curly = consume(Token::Type::RCURLY);

		return new AST(
			AST::Type::INITIALISER_LIST,
			l_curly->source_position.merge(r_curly.value()->source_position),
			InitialiserListAST(list_items)
		);
	}
}