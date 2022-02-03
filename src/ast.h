#pragma once

#include <optional>
#include <variant>
#include <vector>
#include <token.h>
#include <type.h>
#include <sstream>
#include <spdlog/spdlog.h>
#include <trove.h>

namespace trove {

	struct AST;

	struct ProgramAST {
		ProgramAST() {}
		ProgramAST(std::vector<AST*> body) : body(body) {}
		std::string to_string();
		std::vector<AST*> body;
	};

	struct BlockAST {
		BlockAST() {}
		BlockAST(std::vector<AST*> body) : body(body) {}
		std::string to_string();
		std::vector<AST*> body;
	};

	struct StatementAST {
		StatementAST() {}
		StatementAST(AST* body) : body(body) {}
		std::string to_string();
		AST* body;
	};

	struct WatchmanAST {
		WatchmanAST() {}
		WatchmanAST(AST* lhs, AST* body) : lhs(lhs), body(body) {}
		std::string to_string();
		AST* lhs;
		AST* body;
	};

	struct CompAST {
		CompAST() {}
		CompAST(AST* stmt) : stmt(stmt) {}
		AST* get_stmt() {
			return stmt;
		}
		AST* stmt;
	};

	struct FnAST {
		FnAST() {}
		FnAST(AST* body, std::vector<AST*> params, Type return_type, u32 local_count, Type type) 
			: body(body), params(params), return_type(return_type), local_count(local_count), type(type) {}
		std::string to_string();
		AST* body;
		std::vector<AST*> params;
		u32 local_count = 0;
		Type return_type;
		Type type;
	};

	struct StringAST {
		StringAST() {}
		StringAST(Token* token) : token(token) {
		}
		std::string to_string();
		Token* get_token() {
			return token;
		}
		Token* token;
	};

	struct NumAST {
		NumAST() {}
		NumAST(Token* token, Type type) : token(token), type(type) {
		}
		std::string to_string();
		Token* token;
		Type type;
	};

	struct BoolAST {

		enum class Type {
			FALSY,
			TRUEY
		};

		BoolAST() {}
		BoolAST(Type t, trove::Type type) : t(t), type(type) {
		}
		std::string to_string();
		Type t;
		trove::Type type;
	};

	struct VarAST {

		enum class Type : u8 {
			LOCAL,
			PARAM,
			GLOBAL
		};
		
		VarAST() {}
		VarAST(Token* token) : token(token) {}
		std::string to_string();
		Token* get_token() {
			return token;
		}
		Type& get_type() {
			return type;
		}
		Token* token;
		Type type;
	};

	struct DeclAST {
		DeclAST() {}
		DeclAST(Token* token) : token(token) {}
		DeclAST(Token* token, std::optional<Type> type) : token(token), type(type) {}
		DeclAST(Token* token, std::optional<Type> type, AST* value, u1 requires_infering) 
			: token(token), type(type), value(value), requires_infering(requires_infering) {}
		std::string to_string();
		Token* token;
		// fixme this should be non-optional
		std::optional<Type> type;
		std::optional<AST*> value; 
		u1 requires_infering;
	};

	struct CallAST {
		CallAST() {}
		CallAST(AST* callee, std::vector<AST*> args) : callee(callee), args(args) {}
		std::string to_string();
		AST* callee;
		std::vector<AST*> args;
	};

	struct UnAST {

		enum class Type : u8 {
			LOGICAL_NOT,
			TAKE_REFERENCE
		};

		UnAST() {}
		UnAST(Token* op, AST* value)
			: op(op), value(value) {}
		std::string to_string();
		Token* op;
		AST* value;
	};
	struct BinAST {

		static const char* type_lookup[4];
		
		enum class Type : u8 {
			ADD,
			SUB,
			MUL,
			DIV
		};
		BinAST() {}
		BinAST(AST* lhs, Type type, AST* rhs)
			: lhs(lhs), type(type), rhs(rhs) {}
		std::string to_string();
		AST* lhs;
		Type type;
		AST* rhs;
	};

	struct IfAST {
		IfAST() {}
		IfAST(AST* cond, AST* body)
			: cond(cond), body(body) {}
		IfAST(AST* cond, AST* body, AST* else_body)
			: cond(cond), body(body), else_body(else_body) {}
		std::string to_string();
		AST* cond;
		AST* body;
		std::optional<AST*> else_body;
	};

	struct RetAST {
		RetAST() {}
		RetAST(AST* value) : value(value) {}
		AST* value;
	};

	struct StructDefAST {
		// FIXME a struct def ast should not have a type because it is a struct def! what else can it be?!
		// fixme this should not have a type associated with it (it should be infered when generating)
		StructDefAST(){
			type = TypeBuilder::builder()
				.base_type(Type::BaseType::TYPE)
				.build();
		}
		StructDefAST(std::vector<AST*> member_decls) : member_decls(member_decls) {
			type = TypeBuilder::builder()
				.base_type(Type::BaseType::TYPE)
				.build();
		}
		std::string to_string();
		std::vector<AST*> member_decls;
		Type type;
	};

	struct StructLiteralAST {
		StructLiteralAST() {}
		StructLiteralAST(std::vector<AST*> member_values) : member_values(member_values) {
			this->type = TypeBuilder{}
				.base_type(Type::BaseType::STRUCT)
				.anonymous(true)
				.build();
		}
		std::string to_string();
		std::vector<AST*> member_values;
		Type type;
	};


	struct StructAccessAST {
		StructAccessAST() {}
		StructAccessAST(AST* obj, Token* member) : obj(obj), member(member) {}
		StructAccessAST(AST* obj, u32 index) : obj(obj), index(index) {}
		AST* obj;
		Token* member;
		u32 index = 0;
	};

	struct AssignAST {
		AssignAST() {}
		AssignAST(AST* assignee, AST* value) : assignee(assignee), value(value) {}
		std::string to_string();
		AST* assignee;
		AST* value;
	};

	struct LoopAST {
		enum class LoopType : u8 {
			BASIC
		};
		LoopAST() {}
		LoopAST(LoopType loop_type, AST* cond, AST* body) : loop_type(loop_type), cond(cond), body(body) {}
		std::string to_string();
		AST* cond;
		AST* body;
		LoopType loop_type;
	};

	struct InitialiserListAST {
		InitialiserListAST() {}
		InitialiserListAST(std::vector<AST*> values) : values(values){}
		std::string to_string();
		std::vector<AST*> values;
	};

	struct ModuleAST {
		ModuleAST() {}
		ModuleAST(std::vector<AST*> body) : body(body) {
			type = TypeBuilder::builder()
				.base_type(Type::BaseType::MODULE)
				.build();
		}
		std::string to_string();
		std::vector<AST*> body;
		Type type;
	};


	struct AST {

		enum class Type : u8 {
			PROGRAM,
			BLOCK,
			STATEMENT,
			WATCHMAN,
			STRUCT_DEF,
			STRUCT_LITERAL,
			STRUCT_ACCESS,
			FN,
			NUM,
			BOOL,
			VAR,
			STRING,
			CALL,
			UN,
			BIN,
			DECL,
			ASSIGN,
			IF,
			RET,
			COMP,
			LOOP,
			INITIALISER_LIST,
			MODULE
		};

		using ASTValue = std::variant<
			ProgramAST,
			BlockAST,
			StatementAST,
			WatchmanAST,
			CompAST,
			FnAST,
			NumAST,
			BoolAST,
			VarAST,
			StringAST,
			UnAST,
			BinAST,
			DeclAST,
			AssignAST,
			CallAST,
			IfAST,
			RetAST,
			StructDefAST,
			StructLiteralAST,
			StructAccessAST,
			InitialiserListAST,
			LoopAST,
			ModuleAST
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

		StatementAST& as_statement() {
			return std::get<StatementAST>(value);
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

		BoolAST& as_bool() {
			return std::get<BoolAST>(value);
		}

		StringAST& as_str() {
			return std::get<StringAST>(value);
		}

		CallAST& as_call() {
			return std::get<CallAST>(value);
		}

		UnAST& as_un() {
			return std::get<UnAST>(value);
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

		InitialiserListAST& as_initialiser_list() {
			return std::get<InitialiserListAST>(value);
		}

		ModuleAST& as_module() {
			return std::get<ModuleAST>(value);
		}
		
		SourcePosition& get_position() {
			return source_position;
		}

		std::string to_string() {
			switch (type) {
			case Type::PROGRAM: return as_program().to_string();
			case Type::BLOCK: return as_block().to_string();
			case Type::STATEMENT: return as_statement().to_string();
			case Type::DECL: return as_decl().to_string();
			case Type::ASSIGN: return as_assign().to_string();
			case Type::BIN: return as_bin().to_string();
			case Type::VAR: return as_var().to_string();
			case Type::NUM: return as_num().to_string();
			case Type::STRING: return as_str().to_string();
			case Type::FN: return as_fn().to_string();
			case Type::STRUCT_DEF: return as_struct_def().to_string();
			case Type::STRUCT_LITERAL: return as_struct_literal().to_string();
			case Type::CALL: return as_call().to_string();
			case Type::LOOP: return as_loop().to_string();
			case Type::IF: return as_if().to_string();
			case Type::BOOL: return as_bool().to_string();
			default: return "Unknown";
			}
		}
		SourcePosition source_position;
		Type type;
		ASTValue value;
		AST* parent = nullptr;
	};

	extern const char* ast_type_debug[];

}