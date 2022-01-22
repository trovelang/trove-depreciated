#include <lexer.h>
#include <sstream>
#include <spdlog/spdlog.h>

namespace trove {
	std::vector<Token> Lexer::lex() {


		while (!end()) {
			skip_whitespace();
			auto current = next();
			switch (current) {
				// @TODO these are sequential i.e. \r\n is a newline
			case '\n': index = 0; line++; reset_save_point(); break;
			case '\r': index = 0; line++; reset_save_point(); break;
			case '#': token(Token::Type::HASH); break;
			case '@': token(Token::Type::DIRECTIVE); break;
			case '+': token(Token::Type::PLUS); break;
			case '-': token(Token::Type::MINUS); break;
			case '*': token(Token::Type::STAR); break;
			case '(': token(Token::Type::LPAREN); break;
			case ')': token(Token::Type::RPAREN); break;
			case '[': token(Token::Type::LBRACKET); break;
			case ']': token(Token::Type::RBRACKET); break;
			case '{': token(Token::Type::LCURLY); break;
			case '}': token(Token::Type::RCURLY); break;
				//case '_': token(Token::Type::UNDERSCORE); break;
			case ';': token(Token::Type::SEMI_COLON); break;
			case '^': token(Token::Type::POINTER); break;
			case ',': token(Token::Type::COMMA); break;

			case '&': decide(Token::Type::BAND, Token::Type::LAND); break;
			case '|': decide(Token::Type::PIPE, Token::Type::LOR); break;
			case '!': decide(Token::Type::BANG, Token::Type::NEQ); break;
			case '=': decide(Token::Type::ASSIGN, Token::Type::EQUALS); break;
			case ':': decide(Token::Type::COLON, Token::Type::QUICK_ASSIGN); break;

			case '.': decide(Token::Type::DOT, Token::Type::DOUBLE_DOT, Token::Type::TRIPLE_DOT); break;
			case '>': decide(Token::Type::GREATER, Token::Type::GREATER_EQ, Token::Type::RSHIFT); break;
			case '<': decide(Token::Type::LESS, Token::Type::LESS_EQ, Token::Type::LSHIFT); break;


			case '/': do_comment(); break;

			default: {
				if (is_letter(current) || current == '_') {
					do_word(current);
				}
				else if (is_num(current)) {
					do_number(current);
				}
				else if (is_string(current)) {
					do_string(current);
				}
			}

			}
		}

		tokens.push_back(Token(Token::Type::END, { 0,0,0,0 }));
		return tokens;
	}

	u1 Lexer::end() {
		return current >= source.size();
	}

	char Lexer::advance(u32 amount) {
		index += amount;
		char c = source[current];
		current += amount;
		return c;
	}
	char Lexer::peek(u32 ahead = 0) {
		return source[current + ahead];
	}
	char Lexer::next() {
		return advance(1);
	}
	char Lexer::prev() {
		return source[--current];
	}
	void Lexer::reset_save_point() {
		index_save_point = index;
		line_save_point = line;
	}
	void Lexer::skip_whitespace() {
		while (peek() == ' ' || peek() == '\t') {
			next();
			reset_save_point();
		}
	}
	void Lexer::token(Token::Type type) {
		auto token = Token(type, SourcePosition{ index_save_point, index, line_save_point, line });
		//spdlog::info("{} {} {} {} {}", token.to_string(), index_save_point, index, line_save_point, line);
		tokens.push_back(token);
	}
	void Lexer::token(Token::Type type, std::string value) {
		auto token = Token(type, SourcePosition{ index_save_point, index, line_save_point, line }, value);
		//spdlog::info("{} {} {} {} {}", token.to_string(), index_save_point, index, line_save_point, line);
		tokens.push_back(token);
	}

	void Lexer::decide(Token::Type type1, Token::Type type2) {
		return token(type1);
		// todo this is broken with ==

		if (!end() && peek() == '=') {
			next();
			return token(type2);
		}
		if (!end() && prev() == peek()) {
			next();
			return token(type2);
		}
		else
			token(type1);
	}

	void Lexer::decide(Token::Type type1, Token::Type type2, Token::Type type3) {
		if (!end() && peek() != '=' && peek() != prev()) {
			next();
			return token(type1);
		}

		if (!end() && peek() == '=') {
			advance(1);
			return token(type2);
		}
		// e.g. >> or ||
		if (!end() && prev() == peek()) {
			advance(1);
			return token(type3);
		}
	}

	u1 Lexer::is_letter(char current) {
		return current >= 'a' && current <= 'z';
	}
	u1 Lexer::is_num(char current) {
		return current >= '0' && current <= '9';
	}
	u1 Lexer::is_string(char current) {
		return current == '\'' || current == '"';
	}

	u1 Lexer::check_keyword(std::string rest, Token::Type t) {
		int i = 0;
		for (i; i < rest.size() && !end(); i++)
			if (peek(i) != rest.at(i))
				return 0;
		if (!(current + i > source.length() - 1) && (is_letter(peek(i)) || is_num(peek(i)) || peek(i) == '_'))
			return 0;
		advance((u32)rest.size());
		token(t);
		return 1;
	}

	void Lexer::do_word(char current) {
		u1 found_keyword = false;


		switch (current) {
		case 'c': {
			found_keyword = check_keyword("omp", Token::Type::COMP);
			if (!found_keyword) found_keyword = check_keyword("onst", Token::Type::CONST);
			break;
		}
		case 'e': found_keyword = check_keyword("lse", Token::Type::ELSE); break;
		case 'f': {
			found_keyword = check_keyword("n", Token::Type::FN);
			if (!found_keyword) found_keyword = check_keyword("or", Token::Type::FOR);
			if (!found_keyword) found_keyword = check_keyword("alse", Token::Type::FALSE);
			break;
		}
		case 'i': {
			found_keyword = check_keyword("f", Token::Type::IF);
			/*if (!found_keyword) found_keyword = check_keyword("nterface", Token::Type::INTERFACE);
			if (!found_keyword) found_keyword = check_keyword("n", Token::Type::INN);
			if (!found_keyword) found_keyword = check_keyword("nclude", Token::Type::INCLUDE);
			if (!found_keyword) found_keyword = check_keyword("mport", Token::Type::IMPORT);*/
			break;
		}
		case 'l': {
			found_keyword = check_keyword("oop", Token::Type::LOOP);
			break;
		}
		case 'p': {
			found_keyword = check_keyword("ub", Token::Type::PUB);
			if (!found_keyword) found_keyword = check_keyword("riv", Token::Type::PRIV);
			break;
		}
		case 'r': {
			found_keyword = check_keyword("et", Token::Type::RET);
			break;
		}
		case 's': {
			found_keyword = check_keyword("32", Token::Type::S32);
			if (!found_keyword) found_keyword = check_keyword("truct", Token::Type::STRUCT);
			if (!found_keyword) found_keyword = check_keyword("tring", Token::Type::STRING);
			break;
		}
		case 't': {
			found_keyword = check_keyword("ype", Token::Type::TYPE);
			if (!found_keyword) found_keyword = check_keyword("rue", Token::Type::TRUE);
			break;
		}
		case 'u': {
			found_keyword = check_keyword("32", Token::Type::U32);
			break;
		}
		case 'v': {
			found_keyword = check_keyword("ar", Token::Type::VAR);
			break;
		}
		}

		/*

		switch (current) {
		case 'a': {
			found_keyword = check_keyword("s", Token::Type::AS); break;
			if (!found_keyword) found_keyword = check_keyword("nd", Token::Type::LAND);
		}
		case 'b': found_keyword = check_keyword("reak", Token::Type::BREAK); break;
		case 'c': {
			found_keyword = check_keyword("har", Token::Type::CHAR);
			if (!found_keyword) found_keyword = check_keyword("ontinue", Token::Type::CONTINUE);
			break;
		}
		case 'd': found_keyword = check_keyword("efer", Token::Type::DEFER); break;
		case 'e': found_keyword = check_keyword("lse", Token::Type::ELSE); break;
		case 'f': {
			found_keyword = check_keyword("32", Token::Type::F32);
			if (!found_keyword) found_keyword = check_keyword("64", Token::Type::F64);
			if (!found_keyword) found_keyword = check_keyword("or", Token::Type::FOR);
			if (!found_keyword) found_keyword = check_keyword("alse", Token::Type::FLSE);
			if (!found_keyword) found_keyword = check_keyword("n", Token::Type::FN);
			break;
		}
		case 'i': {
			found_keyword = check_keyword("f", Token::Type::IF);
			if (!found_keyword) found_keyword = check_keyword("nterface", Token::Type::INTERFACE);
			if (!found_keyword) found_keyword = check_keyword("n", Token::Type::INN);
			if (!found_keyword) found_keyword = check_keyword("nclude", Token::Type::INCLUDE);
			if (!found_keyword) found_keyword = check_keyword("mport", Token::Type::IMPORT);
			break;
		}
		case 'm': {
			found_keyword = check_keyword("odule", Token::Type::MODULE); break;
		}
		case 'o': {
			found_keyword = check_keyword("r", Token::Type::LOR); break;
		}
		case 'r': {
			found_keyword = check_keyword("et", Token::Type::RETURN);
			if (!found_keyword) found_keyword = check_keyword("un", Token::Type::RUN);
			break;
		}
		case 's': {
			found_keyword = check_keyword("8", Token::Type::S8);
			if (!found_keyword) found_keyword = check_keyword("32", Token::Type::S32);
			if (!found_keyword) found_keyword = check_keyword("65", Token::Type::S64);
			if (!found_keyword) found_keyword = check_keyword("tring", Token::Type::STRING);
			break;
		}
		case 't': {
			found_keyword = check_keyword("rue", Token::Type::TRU);
			if (!found_keyword) found_keyword = check_keyword("ype", Token::Type::TYPE);
			if (!found_keyword) found_keyword = check_keyword("ypeof", Token::Type::TYPEOF);
			break;
		}
		case 'u': {
			found_keyword = check_keyword("8", Token::Type::U8);
			if (!found_keyword) found_keyword = check_keyword("16", Token::Type::U16);
			if (!found_keyword) found_keyword = check_keyword("32", Token::Type::U32);
			if (!found_keyword) found_keyword = check_keyword("64", Token::Type::U64);
			if (!found_keyword) found_keyword = check_keyword("se", Token::Type::USE);
			break;
		}
		case 'x': found_keyword = check_keyword("or", Token::Type::LAND); break;
		}
		*/
		if (!found_keyword) {
			std::stringstream ss;
			ss << current;
			while (!end() && (is_letter(peek()) || is_num(peek()) || peek() == '_'))
				ss << next();
			token(Token::Type::IDENTIFIER, ss.str());
		}
	}

	void Lexer::do_comment() {
	}

	void Lexer::do_number(char current) {
		std::stringstream ss;
		ss << current;
		while (!end() && (is_num(peek()) || peek() == '.' || peek() == '_'))
			ss << next();
		token(Token::Type::NUM, ss.str());
	}
	void Lexer::do_string(char current) {
		std::stringstream ss;
		while (!end() && peek() != current) {
			auto c = next();
			// check for escape sequences
			if (c == '\\') {
				switch (next()) {
				case 'n': {ss << '\n'; break; }
				case 'r': {ss << '\r'; break; }
				case '\'': {ss << '\''; break; }
				case '\t': {ss << '\t'; break; }
				}
			}
			else {
				ss << c;
			}
		}
		// consume past the delimiter
		next();
		token(Token::Type::STRING, ss.str());
	}

}