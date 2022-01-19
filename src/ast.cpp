#include <ast.h>

namespace trove {

	const char* ast_type_debug[] = {
		"AST_PROGRAM",
		"AST_BLOCK",
		"AST_WATCHMAN",
		"AST_STRUCT_DEF",
		"AST_STRUCT_LITERAL",
		"AST_STRUCT_ACCESS",
		"AST_FN",
		"AST_NUM",
		"AST_VAR",
		"AST_STRING",
		"AST_CALL",
		"AST_BIN",
		"AST_DECL",
		"AST_ASSIGN",
		"AST_IF",
		"AST_RET",
		"AST_LOOP"
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


	std::string WatchmanAST::to_string() {
		std::stringstream ss;
		ss << "WatchmanAST\n";
		ss << "{\n";
		ss << "\n}";
		return ss.str();
	}

	std::string DeclAST::to_string() {
		std::stringstream ss;
		ss << "DeclAST " << token->get_value() << " " << type.to_string();
		if (value.has_value()) {
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
		const char* type_lookup[4] = {
			"+", "-", "*", "/"
		};
		std::stringstream ss;
		ss << "BinAST " << lhs->to_string() << " " << type_lookup[(int)type] << " " << rhs->to_string() << "\n";
		return ss.str();
	}

	std::string VarAST::to_string() {
		std::stringstream ss;
		ss << "VarAST " << token->get_value();
		return ss.str();
	}

	std::string NumAST::to_string() {
		std::stringstream ss;
		ss << "NumAST " << token->get_value();
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

}