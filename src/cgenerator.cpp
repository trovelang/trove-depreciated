#include <cgenerator.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

namespace trove {

	std::string CGenerator::execute(const char* cmd) {
		char buffer[128];
		std::string result = "";

		// Open pipe to file
		FILE* pipe = _popen(cmd, "r");
		if (!pipe) {
			return "popen failed!";
		}

		// read till end of process:
		while (!feof(pipe)) {

			// use buffer to read and add to result
			if (fgets(buffer, 128, pipe) != NULL)
				result += buffer;
		}

		_pclose(pipe);
		return result;
	}

	std::string CGenerator::type_to_str(Type type){
		switch (type.get_type()) {
		case TypeType::U32: return "unsigned int";
		case TypeType::S32: return "int";
		case TypeType::FN: return "fn";
		}
	}

	void CGenerator::emit(std::string code) {
		ss << code;
	}

	void CGenerator::gen() {
		gen(ast);
		auto code = ss.str();
		std::ofstream myfile;
		myfile.open("c:/trovelang/trove/tmp/tmp.c");
		myfile << code;
		myfile.close();
		system("gcc c:/trovelang/trove/tmp/tmp.c -o c:/trovelang/trove/tmp/tmp.exe");
		execute("c:/trovelang/trove/tmp/tmp.exe");
	}

	void CGenerator::gen(AST* ast) {
		switch (ast->get_type()) {
		case AST::Type::PROGRAM: gen(ast->as_program()); break;
		case AST::Type::BLOCK: gen(ast->as_block()); break;
		case AST::Type::DECL: gen(ast->as_decl()); break;
		case AST::Type::ASSIGN: gen(ast->as_assign()); break;
		case AST::Type::BIN: gen(ast->as_bin()); break;
		case AST::Type::NUM: gen(ast->as_num()); break;
		case AST::Type::VAR: gen(ast->as_var()); break;
		case AST::Type::FN: gen(ast->as_fn()); break;
		}
	}

	void CGenerator::gen(ProgramAST& ast) {
		emit("#include <stdio.h>\n");
		for (auto& expr : ast.get_body()) {
			gen(expr);
		}
	}

	void CGenerator::gen(BlockAST& ast) {
		emit("{\n");
		for (auto& expr : ast.get_body()) {
			gen(expr);
		}
		emit("}\n");
	}

	void CGenerator::gen(DeclAST& ast) {

		// we are doing a global fn
		// todo check for global
		if (ast.get_type().value().get_type() == TypeType::FN 
			&& ast.get_type().value().get_mutability_modifier()==MutabilityModifier::CONST){
			gen(ast.get_value().value());
		}
		else {
			emit(type_to_str(ast.get_type().value()));
			emit(" ");
			emit(ast.get_token()->get_value());
			if (ast.get_value().has_value()) {
				emit(" = ");
				gen(ast.get_value().value());
			}
			emit(";\n");
			//emit("printf(\"%d\", ");
			//emit(ast.get_token()->get_value());
			//emit(");\n");
		}
	}

	void CGenerator::gen(AssignAST& ast) {
		gen(ast.assignee);
		emit(" = ");
		gen(ast.value);
		emit(";\n");
	}

	void CGenerator::gen(BinAST& ast) {
		gen(ast.get_lhs());
		emit(" ");
		emit(BinAST::type_lookup[(int)ast.get_type()]);
		emit(" ");
		gen(ast.get_rhs());
	}

	void CGenerator::gen(FnAST& fn_ast) {
		emit("void ");
		emit(fn_ast.get_type().get_token()->get_value());
		//emit("(){\n");
		emit("(");
		for (u32 i=0;i<fn_ast.get_params().size();i++) {
			gen(fn_ast.get_params()[i]);
			if (i < fn_ast.get_params().size() - 1) {
				emit(",");
			}
		}
		emit("){\n");
		gen(fn_ast.get_body());
		emit("}\n");
	}
	
	void CGenerator::gen(NumAST& ast) {
		emit(ast.get_token()->get_value());
	}

	void CGenerator::gen(VarAST& ast) {
		emit(ast.get_token()->get_value());
	}
}