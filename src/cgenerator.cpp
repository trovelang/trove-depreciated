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
		CGeneratorContext ctx;
		gen(ctx, ast);
		auto code = ss.str();
		std::ofstream myfile;
		myfile.open("c:/trovelang/trove/tmp/tmp.c");
		myfile << code;
		myfile.close();
		system("gcc c:/trovelang/trove/tmp/tmp.c -o c:/trovelang/trove/tmp/tmp.exe");
		execute("c:/trovelang/trove/tmp/tmp.exe");
	}

	void CGenerator::gen(CGeneratorContext& ctx, AST* ast) {
		switch (ast->get_type()) {
		case AST::Type::PROGRAM: gen(ctx, ast->as_program()); break;
		case AST::Type::BLOCK: gen(ctx, ast->as_block()); break;
		case AST::Type::DECL: gen(ctx, ast->as_decl()); break;
		case AST::Type::ASSIGN: gen(ctx, ast->as_assign()); break;
		case AST::Type::BIN: gen(ctx, ast->as_bin()); break;
		case AST::Type::NUM: gen(ctx, ast->as_num()); break;
		case AST::Type::STRING: gen_string(ctx, ast); break;
		case AST::Type::VAR: gen(ctx, ast->as_var()); break;
		case AST::Type::FN: gen(ctx, ast->as_fn()); break;
		case AST::Type::CALL: gen_call(ctx, ast); break;
		case AST::Type::LOOP: gen_loop(ctx, ast); break;
		case AST::Type::BOOL: gen_bool(ctx, ast); break;
		}
	}

	void CGenerator::gen(CGeneratorContext& ctx, ProgramAST& ast) {
		emit("#include <stdio.h>\n");
		for (auto& expr : ast.get_body()) {
			gen(ctx, expr);
		}
	}

	void CGenerator::gen(CGeneratorContext& ctx, BlockAST& ast) {
		emit("{\n");
		for (auto& expr : ast.get_body()) {
			gen(ctx, expr);
		}
		emit("}\n");
	}

	void CGenerator::gen(CGeneratorContext& ctx, DeclAST& ast) {

		// we are doing a global fn
		// todo check for global
		if (ast.get_type().value().get_type() == TypeType::FN 
			&& ast.get_type().value().get_mutability_modifier()==MutabilityModifier::CONST){
			spdlog::info("generating fn!");
			gen(ctx, ast.get_value().value());
		}else if (ast.get_type().value().get_type() == TypeType::FN
			&& ast.get_type().value().get_mutability_modifier() == MutabilityModifier::MUT) {
			spdlog::info("generating lambda!");
			gen(ctx, ast.get_value().value());
			emit("void (*");
			emit(ast.get_token()->get_value());
			emit(")(");

			auto params = ast.get_type().value().multiple;
			// do the type params
			for (u32 i = 0; i < params.size(); i++) {
				emit(type_to_str(params[i]));
				if (i < params.size() - 1) {
					emit(", ");
				}
			}

			emit(") = &");
			emit(ast.get_type().value().get_token()->get_value());
			emit(";");
		}else {
			emit(type_to_str(ast.get_type().value()));
			emit(" ");
			emit(ast.get_token()->get_value());
			if (ast.get_value().has_value()) {
				emit(" = ");
				gen(ctx, ast.get_value().value());
			}
			emit(";\n");
			//emit("printf(\"%d\", ");
			//emit(ast.get_token()->get_value());
			//emit(");\n");
		}
	}

	void CGenerator::gen(CGeneratorContext& ctx, AssignAST& ast) {
		gen(ctx, ast.assignee);
		emit(" = ");
		gen(ctx, ast.value);
		emit(";\n");
	}

	void CGenerator::gen(CGeneratorContext& ctx, BinAST& ast) {
		gen(ctx, ast.get_lhs());
		emit(" ");
		emit(BinAST::type_lookup[(int)ast.get_type()]);
		emit(" ");
		gen(ctx, ast.get_rhs());
	}

	void CGenerator::gen(CGeneratorContext& ctx, FnAST& fn_ast) {
		emit("void ");
		emit(fn_ast.get_type().get_token()->get_value());
		//emit("(){\n");
		emit("(");
		for (u32 i = 0; i < fn_ast.get_params().size(); i++) {
			gen(ctx, fn_ast.get_params()[i]);
			if (i < fn_ast.get_params().size() - 1) {
				emit(",");
			}
		}
		emit("){\n");
		gen(ctx, fn_ast.get_body());
		emit("}\n");
	}
	
	void CGenerator::gen(CGeneratorContext&, NumAST& ast) {
		emit(ast.get_token()->get_value());
	}

	void CGenerator::gen(CGeneratorContext&, VarAST& ast) {
		emit(ast.get_token()->get_value());
	}

	void CGenerator::gen_call(CGeneratorContext& ctx, AST* ast) {
		gen(ctx, ast->as_call().get_callee());
		emit("(");
		for (u32 i = 0; i < ast->as_call().get_args().size(); i++) {
			gen(ctx, ast->as_call().get_args()[i]);
			if(i<ast->as_call().get_args().size()-1){
				emit(", ");
			}
		}
		emit(");");
	}

	void CGenerator::gen_string(CGeneratorContext& ctx, AST* ast) {
		emit("\"");
		emit(ast->as_str().token->get_value());
		emit("\"");
	}

	void CGenerator::gen_bool(CGeneratorContext& ctx, AST* ast) {
		emit(ast->as_bool().token->get_value());
	}

	void CGenerator::gen_loop(CGeneratorContext& ctx, AST* ast) {
		emit("for (int i=0;i<10;i++)");
		gen(ctx, ast->as_loop().get_body());
	}
}