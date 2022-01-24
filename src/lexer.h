#pragma once

#include <vector>
#include <string_view>
#include <token.h>

namespace trove {
	
	class Lexer {
	public:
		Lexer() {}
		Lexer(std::string_view source) : m_source(source) {}
		std::vector<Token> lex();
	private:
		u1 end();
		char advance(u32 amount);
		char peek(u32 ahead);
		char next();
		char prev();
		void reset_save_point();
		void skip_whitespace();
		void token(Token::Type type);
		void token(Token::Type type, std::string value);
		void decide(Token::Type type1, Token::Type type2);
		void decide(Token::Type type1, Token::Type type2, Token::Type type3);
		void do_comment();
		u1 check_keyword(std::string rest, Token::Type t);
		u1 is_letter(char current);
		u1 is_num(char current);
		u1 is_string(char current);
		void do_word(char current);
		void do_number(char current);
		void do_string(char current);

		u32 m_current = 0;
		u32 m_index = 0;
		u32 m_index_save_point = 0;
		u32 m_line = 0;
		u32 m_line_save_point = 0;
		std::string_view m_source;
		std::vector<Token> m_tokens;
	};

}