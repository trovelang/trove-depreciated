#include <cgenerator.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

namespace trove {


	void CGenerator::emit_c_str(std::string str) {
		emit_raw("\"");
		emit_raw(str);
		emit_raw("\"");
	}

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
		case TypeType::TYPE: return "struct";
		case TypeType::STRUCT: {
			if (type.anonymous) {
				// if we are an anonymous type we need to define the members
				std::stringstream ss;
				ss << "struct {";
				for (u32 i = 0; i < type.multiple.size(); i++) {
					ss << type_to_str(type.multiple[i]) << " member_" << i << ";";
				}
				ss << "}";
				return ss.str();
			}
			else {
				return std::string("struct ").append(type.token->get_value());
			}
		}
		}
	}

	void CGenerator::emit_raw(std::string code) {
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
		case AST::Type::STATEMENT: gen_statement(ctx, ast); break;
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
		case AST::Type::STRUCT_DEF: gen_struct_def(ctx, ast); break;
		case AST::Type::STRUCT_LITERAL: gen_struct_literal(ctx, ast); break;
		}
	}

	void CGenerator::gen_runtime() {
		emit_raw("#include <stdio.h>\n");
		emit_raw("struct __type_info{\nconst char* __type_name;\nconst char* __type_members[];\n};\n");
	}

	void CGenerator::gen(CGeneratorContext& ctx, ProgramAST& ast) {
		gen_runtime();
		for (auto& expr : ast.get_body()) {
			gen(ctx, expr);
		}
	}

	void CGenerator::gen(CGeneratorContext& ctx, BlockAST& ast) {
		emit_raw("{\n");
		for (auto& expr : ast.get_body()) {
			gen(ctx, expr);
		}
		emit_raw("}");
	}

	void CGenerator::gen_statement(CGeneratorContext& ctx, AST* ast) {
		gen(ctx, ast->as_statement().body);
		emit_raw(";\n");
	}

	void CGenerator::gen(CGeneratorContext& ctx, DeclAST& ast) {

		// we are doing a global fn
		// todo check for global
		if (ast.get_type().value().get_type() == TypeType::FN 
			&& ast.get_type().value().get_mutability_modifier()==MutabilityModifier::CONST){
			gen(ctx, ast.get_value().value());
		}else if (ast.get_type().value().get_type() == TypeType::FN
			&& ast.get_type().value().get_mutability_modifier() == MutabilityModifier::MUT) {

			gen(ctx, ast.get_value().value());
			emit_raw("void (*");
			emit_raw(ast.get_token()->get_value());
			emit_raw(")(");

			auto params = ast.get_type().value().multiple;
			// do the type params
			for (u32 i = 0; i < params.size(); i++) {
				emit_raw(type_to_str(params[i]));
				if (i < params.size() - 1) {
					emit_raw(", ");
				}
			}

			emit_raw(") = &");
			emit_raw(ast.get_type().value().get_token()->get_value());
		
		
		}else if (ast.get_type().value().get_type() == TypeType::TYPE
			&& ast.get_type().value().get_mutability_modifier() == MutabilityModifier::CONST) {
			gen(ctx, ast.get_value().value());
		}
		else if (ast.get_type().value().get_type() == TypeType::TYPE
			&& ast.get_type().value().get_mutability_modifier() == MutabilityModifier::MUT) {

			auto t = ast.get_type().value();
			emit_raw("struct __type_info ");
			emit_raw(ast.get_token()->get_value());
			emit_raw("={");
			emit_c_str(t.get_token()->get_value());
			emit_raw(", {");

			auto members = ast.get_value().value()->as_struct_def().get_member_decls();
			for (u32 i = 0; i < members.size(); i++) {
				emit_c_str(members[i]->as_decl().get_token()->get_value());
				if (i < members.size() - 1) {
					emit_raw(",");
				}
			}
			emit_raw("}}");
				

		}
		else {
			emit_raw(type_to_str(ast.get_type().value()));
			emit_raw(" ");
			emit_raw(ast.get_token()->get_value());
			if (ast.get_value().has_value()) {
				emit_raw(" = ");
				gen(ctx, ast.get_value().value());
			}
			//emit("printf(\"%d\", ");
			//emit(ast.get_token()->get_value());
			//emit(");\n");
		}
	}

	void CGenerator::gen(CGeneratorContext& ctx, AssignAST& ast) {
		gen(ctx, ast.assignee);
		emit_raw(" = ");
		gen(ctx, ast.value);
	}

	void CGenerator::gen(CGeneratorContext& ctx, BinAST& ast) {
		gen(ctx, ast.get_lhs());
		emit_raw(" ");
		emit_raw(BinAST::type_lookup[(int)ast.get_type()]);
		emit_raw(" ");
		gen(ctx, ast.get_rhs());
	}

	void CGenerator::gen(CGeneratorContext& ctx, FnAST& fn_ast) {
		emit_raw("void ");
		emit_raw(fn_ast.get_type().get_token()->get_value());
		//emit("(){\n");
		emit_raw("(");
		for (u32 i = 0; i < fn_ast.get_params().size(); i++) {
			gen(ctx, fn_ast.get_params()[i]);
			if (i < fn_ast.get_params().size() - 1) {
				emit_raw(",");
			}
		}
		emit_raw("){\n");
		gen(ctx, fn_ast.get_body());
		emit_raw("}");
	}
	
	void CGenerator::gen(CGeneratorContext&, NumAST& ast) {
		emit_raw(ast.get_token()->get_value());
	}

	void CGenerator::gen(CGeneratorContext&, VarAST& ast) {
		emit_raw(ast.get_token()->get_value());
	}

	void CGenerator::gen_call(CGeneratorContext& ctx, AST* ast) {
		gen(ctx, ast->as_call().get_callee());
		emit_raw("(");
		for (u32 i = 0; i < ast->as_call().get_args().size(); i++) {
			gen(ctx, ast->as_call().get_args()[i]);
			if(i<ast->as_call().get_args().size()-1){
				emit_raw(", ");
			}
		}
		emit_raw(")");
	}

	void CGenerator::gen_string(CGeneratorContext& ctx, AST* ast) {
		emit_raw("\"");
		emit_raw(ast->as_str().token->get_value());
		emit_raw("\"");
	}

	void CGenerator::gen_bool(CGeneratorContext& ctx, AST* ast) {
		emit_raw(ast->as_bool().token->get_value());
	}

	void CGenerator::gen_loop(CGeneratorContext& ctx, AST* ast) {
		emit_raw("for (int i=0;i<10;i++)");
		gen(ctx, ast->as_loop().get_body());
	}

	void CGenerator::gen_struct_def(CGeneratorContext& ctx, AST* ast) {
		
		auto struct_def = ast->as_struct_def();
		emit_raw("struct ");
		emit_raw(struct_def.get_type().token->get_value());
		emit_raw(" {\n");
		for (auto& member : struct_def.get_member_decls()) {
			gen(ctx, member);
			emit_raw(";\n");
		}
		emit_raw("}");

	}

	void CGenerator::gen_struct_literal(CGeneratorContext& ctx, AST* ast) {

		auto struct_literal = ast->as_struct_literal();
		emit_raw("{");
		for (u32 i = 0; i < struct_literal.get_member_values().size();i++) {
			gen(ctx, struct_literal.member_values[i]);
			if (i < struct_literal.member_values.size() - 1) {
				emit_raw(", ");
			}
		}
		emit_raw("}");

	}
}