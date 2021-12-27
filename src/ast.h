#pragma once

#include <optional>
#include <variant>
#include <vector>
#include <token.h>
#include <type.h>

namespace trove {

	class AST;

	class ProgramAST {
	public:
		ProgramAST() {}
		ProgramAST(std::vector<AST*> body) : body(body) {}
		std::vector<AST*>& get_body() {
			return body;
		}
	private:
		std::vector<AST*> body;
	};

	class BlockAST {
	public:
		BlockAST() {}
		BlockAST(std::vector<AST*> body) : body(body) {}
		std::vector<AST*> get_body() {
			return body;
		}
	private:
		std::vector<AST*> body;
	};

	class CompAST {
	public:
		CompAST() {}
		CompAST(AST* stmt) : stmt(stmt) {}
		AST* get_stmt() {
			return stmt;
		}
	private:
		AST* stmt;
	};

	class FnAST {
	public:
		FnAST() {}
		FnAST(AST* body, std::vector<AST*> params, u32 local_count, Type type) : body(body), params(params), local_count(local_count), type(type) {}
		AST* get_body() {
			return body;
		}
		std::vector<AST*>& get_params() {
			return params;
		}
		Type& get_type() {
			return type;
		}
	private:
		AST* body;
		std::vector<AST*> params;
		u32 local_count = 0;
		Type type;
	};

	class NumAST {
	public:
		NumAST() {}
		NumAST(Token* token) : token(token) {
		}
		Token* get_token() {
			return token;
		}
	private:
		Token* token;
	};

	class VarAST {
	public:
		enum class Type : u8 {
			LOCAL,
			PARAM,
			GLOBAL
		};
		VarAST() {}
		VarAST(Token* token) : token(token) {}
		Token* get_token() {
			return token;
		}
		Type& get_type() {
			return type;
		}
	private:
		Token* token;
		Type type;
	};

	class StringAST {
	public:
		StringAST() {}
		StringAST(Token* token) : token(token) {
		}
		Token* get_token() {
			return token;
		}
	private:
		Token* token;
	};

	class DeclAST {
	public:
		DeclAST() {}
		DeclAST(Token* token) : token(token) {}
		DeclAST(Token* token, Type type) : token(token), type(type) {}
		DeclAST(Token* token, Type type, AST* value) : token(token), type(type), value(value) {}
		Token* get_token() {
			return token;
		}
		Type& get_type() {
			return type;
		}
		std::optional<AST*>& get_value() {
			return value;
		}
	private:
		Token* token;
		Type type;
		std::optional<AST*> value;
	};

	class CallAST {
	public:
		CallAST() {}
		CallAST(AST* callee, std::vector<AST*> args) : callee(callee), args(args) {}
		AST* get_callee() {
			return callee;
		}
		std::vector<AST*>& get_args() {
			return args;
		}
	private:
		AST* callee;
		std::vector<AST*> args;
	};

	class BinAST {
	public:
		enum class Type : u8 {
			ADD,
			SUB,
			MUL,
			DIV
		};
		BinAST() {}
		BinAST(AST* lhs, Type type, AST* rhs)
			: lhs(lhs), type(type), rhs(rhs) {}

		AST* get_lhs() {
			return lhs;
		}
		Type& get_type() {
			return type;
		}
		AST* get_rhs() {
			return rhs;
		}
	private:
		AST* lhs;
		Type type;
		AST* rhs;
	};

	class IfAST {
	public:
		IfAST() {}
		IfAST(AST* cond, AST* body)
			: cond(cond), body(body) {}
		IfAST(AST* cond, AST* body, AST* else_body)
			: cond(cond), body(body), else_body(else_body) {}

		AST* get_cond() {
			return cond;
		}
		AST* get_body() {
			return body;
		}
		std::optional<AST*> get_else_body() {
			return else_body;
		}
	private:
		AST* cond;
		AST* body;
		std::optional<AST*> else_body;
	};

	class RetAST {
	public:
		RetAST() {}
		RetAST(AST* value) : value(value) {}
		AST* get_value() {
			return value;
		}
	private:
		AST* value;
	};

	class StructDefAST {
	public:
		// fixme this should not have a type associated with it (it should be infered when generating)
		StructDefAST() : type(TypeType::STRUCT) {}
		StructDefAST(std::vector<AST*> member_decls) : member_decls(member_decls), type(TypeType::STRUCT) {}
		std::vector<AST*>& get_member_decls() {
			return member_decls;
		}
		Type& get_type() {
			return type;
		}
	private:
		std::vector<AST*> member_decls;
		Type type; // either struct or type (defaults to struct)
	};

	class StructLiteralAST {
	public:
		StructLiteralAST() {}
		StructLiteralAST(std::vector<AST*> member_values) : member_values(member_values) {}
		std::vector<AST*>& get_member_values() {
			return member_values;
		}
	private:
		std::vector<AST*> member_values;
	};


	class StructAccessAST {
	public:
		StructAccessAST() {}
		StructAccessAST(AST* obj, Token* member) : obj(obj), member(member) {}
		StructAccessAST(AST* obj, u32 index) : obj(obj), index(index) {}
		AST* get_obj() {
			return obj;
		}
		Token* get_member() {
			return member;
		}
		u32& get_index() {
			return index;
		}
	private:
		AST* obj;
		Token* member;
		u32 index = 0;
	};

	class AssignAST {
	public:
		AssignAST() {}
		AssignAST(AST* assignee, AST* value) : assignee(assignee), value(value) {}
		AST* get_assignee() {
			return assignee;
		}
		AST* get_value() {
			return value;
		}
	private:
		AST* assignee;
		AST* value;
	};

	class LoopAST {
	public:
		enum LoopType : u8 {
			BASIC
		};
		LoopAST() {}
		LoopAST(LoopType loop_type, AST* cond, AST* body) : loop_type(loop_type), cond(cond), body(body) {}

		AST* get_cond() {
			return cond;
		}
		AST* get_body() {
			return body;
		}
		LoopType& get_loop_type() {
			return loop_type;
		}
	private:
		AST* cond;
		AST* body;
		LoopType loop_type;
	};


	class AST {
	public:

		enum Type {
			PROGRAM,
			BLOCK,
			STRUCT_DEF,
			STRUCT_LITERAL,
			STRUCT_ACCESS,
			FN,
			NUM,
			VAR,
			STRING,
			CALL,
			BIN,
			DECL,
			ASSIGN,
			IF,
			RET,
			COMP,
			LOOP
		};

		using ASTValue = std::variant<
			ProgramAST,
			BlockAST,
			CompAST,
			FnAST,
			NumAST,
			VarAST,
			StringAST,
			BinAST,
			DeclAST,
			AssignAST,
			CallAST,
			IfAST,
			RetAST,
			StructDefAST,
			StructLiteralAST,
			StructAccessAST,
			LoopAST
		>;

		AST() {}
		AST(Type type) : type(type) {}
		AST(Type type, SourcePosition source_position, ASTValue value)
			: type(type), source_position(source_position), value(value) {}

		Type& get_type() {
			return type;
		}

		ASTValue& get_value() {
			return value;
		}

		ProgramAST& as_program() {
			return std::get<ProgramAST>(value);
		}

		BlockAST& as_block() {
			return std::get<BlockAST>(value);
		}

		CompAST& as_comp() {
			return std::get<CompAST>(value);
		}

		FnAST& as_fn() {
			return std::get<FnAST>(value);
		}

		IfAST& as_if() {
			return std::get<IfAST>(value);
		}

		RetAST& as_ret() {
			return std::get<RetAST>(value);
		}

		NumAST& as_num() {
			return std::get<NumAST>(value);
		}

		VarAST& as_var() {
			return std::get<VarAST>(value);
		}

		StringAST& as_str() {
			return std::get<StringAST>(value);
		}

		CallAST& as_call() {
			return std::get<CallAST>(value);
		}

		BinAST& as_bin() {
			return std::get<BinAST>(value);
		}

		DeclAST& as_decl() {
			return std::get<DeclAST>(value);
		}

		AssignAST& as_assign() {
			return std::get<AssignAST>(value);
		}

		StructDefAST& as_struct_def() {
			return std::get<StructDefAST>(value);
		}

		StructLiteralAST& as_struct_literal() {
			return std::get<StructLiteralAST>(value);
		}

		StructAccessAST& as_struct_access() {
			return std::get<StructAccessAST>(value);
		}

		LoopAST& as_loop() {
			return std::get<LoopAST>(value);
		}

		SourcePosition& get_position() {
			return source_position;
		}
	private:
		Type type;
		SourcePosition source_position;
		ASTValue value;
	};

	extern const char* ast_type_debug[];

}