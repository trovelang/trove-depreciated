#include <ast.h>
#include <trove.h>

namespace trove {

	const char* BinAST::type_lookup[4] = {
		   "+", "-", "*", "/"
	};

	const char* ast_type_debug[] = {
		"AST_PROGRAM",
		"AST_BLOCK",
		"AST_WATCHMAN",
		"AST_STRUCT_DEF",
		"AST_STRUCT_LITERAL",
		"AST_STRUCT_ACCESS",
		"AST_FN",
		"AST_NUM",
		"AST_BOOL",
		"AST_VAR",
		"AST_STRING",
		"AST_CALL",
		"AST_UN",
		"AST_BIN",
		"AST_DECL",
		"AST_ASSIGN",
		"AST_IF",
		"AST_RET",
		"AST_LOOP",
		"AST_INITIALISER_LIST",
		"AST_MODULE",
		"AST_STRUCT_ACCESS"
	};

	std::string ProgramAST::to_string() {
		std::stringstream ss;
		ss << "ProgramAST\n";
		ss << "{\n";
		for (auto& item : body) {
			ss << item->to_string() << ",\n";
		}
		ss << "\n}";
		return ss.str();
	}

	std::string AnnotationAST::to_string() {
		std::stringstream ss;
		ss << "AnnotationAST " << body->to_string() << "\n";
		return ss.str();
	}

	std::string BlockAST::to_string() {
		std::stringstream ss;
		ss << "BlockAST\n";
		ss << "{\n";
		for (auto& item : body) {
			ss << item->to_string() << ",\n";
		}
		ss << "\n}";
		return ss.str();
	}

	std::string IfAST::to_string() {
		std::stringstream ss;
		ss << "IfAST " << cond->to_string();
		ss << "{\n";
		ss << body->to_string();
		ss << "\n}";
		return ss.str();
	}

	std::string StatementAST::to_string() {
		std::stringstream ss;
		ss << "StatementAST " << body->to_string() << "\n";
		return ss.str();
	}

	std::string WatchmanAST::to_string() {
		std::stringstream ss;
		ss << "WatchmanAST\n";
		ss << "{\n";
		ss << "\n}";
		return ss.str();
	}

	std::string DeclAST::to_string() {
		std::stringstream ss;
		ss << "DeclAST " << token->value << " " << type.value().to_string();
		IF_VALUE(value){
			ss << " " << value.value()->to_string();
		}
		ss << "\n";
		return ss.str();
	}

	std::string AssignAST::to_string() {
		std::stringstream ss;
		ss << "AssignAST " << assignee->to_string() << " " << value->to_string() << "\n";
		return ss.str();
	}

	std::string BinAST::to_string() {
		std::stringstream ss;
		ss << "BinAST " << lhs->to_string() << " " << type_lookup[(int)type] << " " << rhs->to_string() << "\n";
		return ss.str();
	}

	std::string UnAST::to_string() {
		std::stringstream ss;
		ss << "UnAST "<< op->to_string() << value->to_string() << "\n";
		return ss.str();
	}

	std::string LoopAST::to_string() {
		std::stringstream ss;
		ss << "LoopAST " << cond->to_string() << " " << body->to_string() << "\n";
		return ss.str();
	}

	std::string InitialiserListAST::to_string() {
		std::stringstream ss;
		ss << "InitialiserListAST " << "\n";
		return ss.str();
	}

	std::string ModuleAST::to_string() {
		std::stringstream ss;
		ss << "ModuleAST \n";
		ss << "{\n";
		for (auto& item : body) {
			ss << item->to_string() << ",\n";
		}
		ss << "\n}";
		return ss.str();
	}


	std::string VarAST::to_string() {
		std::stringstream ss;
		ss << "VarAST " << token->value;
		return ss.str();
	}

	std::string BoolAST::to_string() {
		const char* bool_lookup[] = {
			"true",
			"false"
		};
		std::stringstream ss;
		ss << "BoolAST " << bool_lookup[(u32)t] << " " << type.to_string();
		return ss.str();
	}

	std::string StringAST::to_string() {
		std::stringstream ss;
		ss << "StringAST " << token->value;
		return ss.str();
	}

	std::string CallAST::to_string() {
		std::stringstream ss;
		ss << "CallAST callee: " << callee->to_string() << " params: ( ";
		for(auto& arg : args){
			ss << arg->to_string() << ", ";
		}
		ss << " )";
		return ss.str();
	}

	std::string NumAST::to_string() {
		std::stringstream ss;
		ss << "NumAST " << token->value;
		return ss.str();
	}

	std::string FnAST::to_string() {
		std::stringstream ss;
		ss << "FnAST " << body->to_string();
		return ss.str();
	}

	std::string StructDefAST::to_string() {
		std::stringstream ss;
		ss << "StructDefAST \n";
		ss << "{\n";
		for (auto& member : member_decls) {
			ss << member->to_string() << ",\n";
		}
		ss << "}\n";
		return ss.str();
	}

	std::string StructLiteralAST::to_string() {
		std::stringstream ss;
		ss << "StructLiteralAST \n";
		ss << "{\n";
		for (auto& member : member_values) {
			ss << member->to_string() << ",\n";
		}
		ss << "}\n";
		return ss.str();
	}

	std::string StructAccessAST::to_string() {
		std::stringstream ss;
		ss << "StructAccessAST "<< obj->to_string() << " " << member->to_string() << "\n";
		return ss.str();
	}

}