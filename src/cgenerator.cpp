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
		switch (type.base_type) {
		case Type::BaseType::INCOMPLETE: return "INCOMPLETE"; // todo assert unreachable
		case Type::BaseType::NONE: return "void";
		case Type::BaseType::STRING: return "const char*";
		case Type::BaseType::BOOL: return "unsigned int";
		case Type::BaseType::U32: return "unsigned int";
		case Type::BaseType::S32: return "int";
		case Type::BaseType::FN: return "fn";
		case Type::BaseType::TYPE: return "struct";
		case Type::BaseType::STRUCT: {
			if (type.struct_type.is_anonymous) {
				// if we are an anonymous type we need to define the members
				std::stringstream ss;
				ss << "struct {";
				for (u32 i = 0; i < type.contained_types.size(); i++) {
					ss << type_to_str(type.contained_types[i]) << " member_" << i << ";";
				}
				ss << "}";
				return ss.str();
			}
			else {
				return std::string("struct ").append(type.associated_token->value);
			}
		}
		}
	}

	void CGenerator::emit_raw(std::string code) {
		m_output_stream << code;
	}

	void CGenerator::gen() {
		CGeneratorContext ctx;
		ctx.in_global = true;
		gen_runtime();
		gen(ctx, m_ast);
		auto code = m_output_stream.str();
		std::ofstream myfile;
		myfile.open("c:/trovelang/trove/tmp/tmp.c");
		myfile << code;
		myfile.close();

		#ifdef CODEGEN_CLANG
		system("clang -w c:/trovelang/trove/tmp/tmp.c -o c:/trovelang/trove/tmp/tmp.exe");
		#endif
		#ifdef CODEGEN_GCC
		system("gcc -w c:/trovelang/trove/tmp/tmp.c -o c:/trovelang/trove/tmp/tmp.exe");
		#endif 
		
		// execute("c:/trovelang/trove/tmp/tmp.exe");
	}

	void CGenerator::gen(CGeneratorContext& ctx, AST* ast) {
		switch (ast->get_type()) {
		case AST::Type::PROGRAM: gen(ctx, ast->as_program()); break;
		case AST::Type::BLOCK: gen(ctx, ast->as_block()); break;
		case AST::Type::STATEMENT: gen_statement(ctx, ast); break;
		case AST::Type::DECL: gen(ctx, ast->as_decl()); break;
		case AST::Type::ASSIGN: gen(ctx, ast->as_assign()); break;
		case AST::Type::UN: gen_un(ctx, ast); break;
		case AST::Type::BIN: gen_bin(ctx, ast); break;
		case AST::Type::NUM: gen(ctx, ast->as_num()); break;
		case AST::Type::RET: gen_ret(ctx, ast); break;
		case AST::Type::STRING: gen_string(ctx, ast); break;
		case AST::Type::VAR: gen(ctx, ast->as_var()); break;
		case AST::Type::FN: gen(ctx, ast->as_fn()); break;
		case AST::Type::CALL: gen_call(ctx, ast); break;
		case AST::Type::LOOP: gen_loop(ctx, ast); break;
		case AST::Type::IF: gen_if(ctx, ast); break;
		case AST::Type::BOOL: gen_bool(ctx, ast); break;
		case AST::Type::STRUCT_DEF: gen_struct_def(ctx, ast); break;
		case AST::Type::STRUCT_LITERAL: gen_struct_literal(ctx, ast); break;
		case AST::Type::STRUCT_ACCESS: gen_struct_access(ctx, ast); break;
		case AST::Type::INITIALISER_LIST: gen_initialiser_list(ctx, ast); break;
		case AST::Type::MODULE: gen_module(ctx, ast); break;
		}
	}

	void CGenerator::gen_runtime() {
		emit_raw("#include <stdio.h>\n");
		emit_raw("struct __type_info{\nconst char* __type_name;\nconst char* __type_members[];\n};\n");
	}

	void CGenerator::gen(CGeneratorContext& ctx, ProgramAST& ast) {
		for (auto& expr : ast.body) {
			gen(ctx, expr);
		}
	}

	void CGenerator::gen(CGeneratorContext& ctx, BlockAST& ast) {
		emit_raw("{\n");
		for (auto& expr : ast.body) {
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
		if (ast.type.value().base_type == Type::BaseType::FN 
			&& ast.type.value().mutability==Type::Mutability::CONSTANT){
			gen(ctx, ast.value.value());
		}else if (ast.type.value().base_type == Type::BaseType::FN
			&& ast.type.value().mutability == Type::Mutability::MUT) {

			gen(ctx, ast.value.value());

			auto return_type = ast.type.value().contained_types.at(ast.type.value().contained_types.size() - 1);

			emit_raw(type_to_str(return_type));
			emit_raw(" (*");
			std::stringstream ss;
			for (auto& mod : ctx.module_ctx.module_names)
				ss << *mod << "_";
			auto variable_name = ss.str().append(ast.token->value);
			emit_raw(variable_name);
			emit_raw(")(");

			auto params = ast.type.value().contained_types;
			// do the type params
			// we have to do -1 because the last is the return type
			for (u32 i = 0; i < params.size()-1; i++) {
				emit_raw(type_to_str(params[i]));
				if (i < params.size() - 2) {
					emit_raw(", ");
				}
			}

			emit_raw(") = &");
			emit_raw(ast.type.value().associated_token->value);
		
		
		}else if (ast.type.value().base_type == Type::BaseType::TYPE
			&& ast.type.value().mutability == Type::Mutability::CONSTANT) {
			gen(ctx, ast.value.value());
		}
		else if (ast.type.value().base_type == Type::BaseType::TYPE
			&& ast.type.value().mutability == Type::Mutability::MUT) {

			auto t = ast.type.value();
			emit_raw("struct __type_info ");
			emit_raw(ast.token->value);
			emit_raw("={");
			emit_c_str(t.associated_token->value);
			emit_raw(", {");

			auto members = ast.value.value()->as_struct_def().member_decls;
			for (u32 i = 0; i < members.size(); i++) {
				emit_c_str(members[i]->as_decl().token->value);
				if (i < members.size() - 1) {
					emit_raw(",");
				}
			}
			emit_raw("}}");
				

		}
		else if (ast.type.value().base_type == Type::BaseType::MODULE
			&& ast.type.value().mutability == Type::Mutability::CONSTANT) {
			emit_raw("// module start\n");

			
			auto new_ctx = ctx;
			new_ctx.in_global = false;
			new_ctx.module_ctx.module_names.push_back(&ast.token->value);

			for (auto& expr : ast.value.value()->as_module().body) {
				gen(new_ctx, expr);
			}

			emit_raw("// module end\n");
		}
		else {
			emit_raw(type_to_str(ast.type.value()));
			emit_raw(" ");
			if (ctx.in_global) {
				emit_raw(ast.token->value);
			}
			else {
				std::stringstream ss;
				for (auto& mod : ctx.module_ctx.module_names)
					ss << *mod << "_";
				auto variable_name = ss.str().append(ast.token->value);
				// put the variable name in the symtable
				emit_raw(variable_name);
			}
			IF_VALUE(ast.value) {
				emit_raw(" = ");
				gen(ctx, ast.value.value());
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

	void CGenerator::gen_un(CGeneratorContext& ctx, AST* ast) {
		if (ast->as_un().op->type == Token::Type::BANG) {
			emit_raw("!");
		}
		gen(ctx, ast->as_un().value);
	}

	void CGenerator::gen_bin(CGeneratorContext& ctx, AST* ast) {
		gen(ctx, ast->as_bin().lhs);
		emit_raw(" ");
		emit_raw(BinAST::type_lookup[(int)ast->as_bin().type]);
		emit_raw(" ");
		gen(ctx, ast->as_bin().rhs);
	}

	void CGenerator::gen_ret(CGeneratorContext& ctx, AST* ast) {
		emit_raw("return ");
		gen(ctx, ast->as_ret().value);
	}

	void CGenerator::gen(CGeneratorContext& ctx, FnAST& fn_ast) {
		emit_raw(type_to_str(fn_ast.type.contained_types.at(fn_ast.type.contained_types.size() - 1)));
		emit_raw(" ");
		std::stringstream ss;
		for (auto& mod : ctx.module_ctx.module_names)
			ss << *mod << "_";
		auto variable_name = ss.str().append(fn_ast.type.associated_token->value);
		emit_raw(variable_name);
		//emit("(){\n");
		emit_raw("(");
		for (u32 i = 0; i < fn_ast.params.size(); i++) {
			gen(ctx, fn_ast.params[i]);
			if (i < fn_ast.params.size() - 1) {
				emit_raw(",");
			}
		}
		emit_raw("){\n");
		gen(ctx, fn_ast.body);
		emit_raw("}");
	}
	
	void CGenerator::gen(CGeneratorContext&, NumAST& ast) {
		emit_raw(ast.token->value);
	}

	void CGenerator::gen(CGeneratorContext&, VarAST& ast) {
		emit_raw(ast.get_token()->value);
	}

	void CGenerator::gen_call(CGeneratorContext& ctx, AST* ast) {
		gen(ctx, ast->as_call().callee);
		emit_raw("(");
		for (u32 i = 0; i < ast->as_call().args.size(); i++) {
			gen(ctx, ast->as_call().args[i]);
			if(i<ast->as_call().args.size()-1){
				emit_raw(", ");
			}
		}
		emit_raw(")");
	}

	void CGenerator::gen_string(CGeneratorContext& ctx, AST* ast) {
		emit_raw("\"");
		emit_raw(ast->as_str().token->value);
		emit_raw("\"");
	}

	void CGenerator::gen_bool(CGeneratorContext& ctx, AST* ast) {
		const char* bool_lookup[] = {
			"0",
			"1"
		};
		emit_raw(bool_lookup[(u32)ast->as_bool().t]);
	}

	void CGenerator::gen_loop(CGeneratorContext& ctx, AST* ast) {
		emit_raw("for (int i=0;i<10;i++)");
		gen(ctx, ast->as_loop().body);
	}

	void CGenerator::gen_if(CGeneratorContext& ctx, AST* ast) {
		emit_raw("if (");
		gen(ctx, ast->as_if().cond);
		emit_raw(")");
		gen(ctx, ast->as_if().body);
		IF_VALUE(ast->as_if().else_body) {
			emit_raw(" else ");
			gen(ctx, ast->as_if().else_body.value());
		}
	}

	void CGenerator::gen_struct_def(CGeneratorContext& ctx, AST* ast) {
		
		auto struct_def = ast->as_struct_def();
		emit_raw("struct ");
		emit_raw(struct_def.type.associated_token->value);
		emit_raw(" {\n");
		for (auto& member : struct_def.member_decls) {
			gen(ctx, member);
			emit_raw(";\n");
		}
		emit_raw("}");

	}

	void CGenerator::gen_struct_literal(CGeneratorContext& ctx, AST* ast) {

		auto struct_literal = ast->as_struct_literal();
		emit_raw("{");
		for (u32 i = 0; i < struct_literal.member_values.size();i++) {
			gen(ctx, struct_literal.member_values[i]);
			if (i < struct_literal.member_values.size() - 1) {
				emit_raw(", ");
			}
		}
		emit_raw("}");

	}

	void CGenerator::gen_struct_access(CGeneratorContext& ctx, AST* ast) {


		// first look up the 
		if (true) {

			// then create the name
			std::stringstream ss;
			ss << ast->as_struct_access().obj->as_var().token->value << "_";
			emit_raw(ss.str());
			gen(ctx, ast->as_struct_access().member);
		}
	}

	void CGenerator::gen_initialiser_list(CGeneratorContext& ctx, AST* ast) {
		emit_raw("{");
		for (u32 i = 0; i < ast->as_initialiser_list().values.size(); i++) {
			gen(ctx, ast->as_initialiser_list().values[i]);
			if (i < ast->as_initialiser_list().values.size() - 1) {
				emit_raw(", ");
			}
		}
		emit_raw("}");
	}

	void CGenerator::gen_module(CGeneratorContext& ctx, AST* ast){
		emit_raw("// anonymous module start\n");
		
		auto new_ctx = ctx;
		// new_ctx.in_global = false;

		// if we are an anonymous module then dont do a name
		// new_ctx.module_ctx.module_names.push_back(&ast.token->value);

		for (auto& expr : ast->as_module().body) {
			gen(new_ctx, expr);
		}

		emit_raw("// anonymous module end\n");
	}
}