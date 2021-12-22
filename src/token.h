#pragma once

#include <log.h>
#include <types.h>
#include <string_view>
#include <string>

class SourcePosition {
public:
	SourcePosition() {}
	SourcePosition(u32 index_start, u32 index_end, u32 line_start, u32 line_end)
		: index_start(index_start), index_end(index_end), line_start(line_start), line_end(line_end) {}

	SourcePosition merge(SourcePosition other) {
		SourcePosition merged;
		merged.index_start = index_start < other.index_start ? index_start : other.index_start;
		merged.index_end = index_end > other.index_end ? index_end : other.index_end;
		merged.line_start = line_start < other.line_start ? line_start : other.line_start;
		merged.line_end = line_end > other.line_end ? line_end : other.line_end;
		return merged;
	}

private:
	u32 index_start = 0;
	u32 index_end = 0;
	u32 line_start = 0;
	u32 line_end = 0;
};

class Token {
public:
	enum Type : u8 {
		END,
		NUM,
		IDENTIFIER,
		STRING,

		HASH,
		DIRECTIVE,

		PLUS,
		MINUS,
		STAR,
		DIV,

		LPAREN,
		RPAREN,
		LBRACKET,
		RBRACKET,
		LCURLY,
		RCURLY,

		SEMI_COLON,
		POINTER,
		COMMA,

		BAND,
		LAND,
		BOR,
		LOR,

		BANG,
		NEQ,

		ASSIGN,
		EQUALS,

		COLON,
		QUICK_ASSIGN,

		DOT,
		DOUBLE_DOT,
		TRIPLE_DOT,

		GREATER,
		GREATER_EQ,

		LESS,
		LESS_EQ,

		RSHIFT,
		LSHIFT,

		FN,
		FOR,
		LOOP,
		IF,
		ELSE,
		RET,

		U32,
		S32,
		TYPE,
		STRUCT,

		COMP

	};

	Token() {}
	Token(Type type, SourcePosition source_position) : type(type), source_position(source_position) {}
	Token(Type type, SourcePosition source_position, std::string value)
		: type(type), source_position(source_position), value(value) {}

	SourcePosition& get_position() {
		return source_position;
	}
	std::string to_string();

	std::string& get_value() {
		return value;
	}

	s32 as_num() {
		return atoi(value.c_str());
	}

	f32 as_float() {}

	Type& get_type() {
		return type;
	}

private:
	Type type;
	SourcePosition source_position;
	std::string value;
};

extern const char* token_type_debug[];
