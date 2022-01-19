#include <cgenerator.h>
#include <spdlog/spdlog.h>

namespace trove {

	std::string CGenerator::type_to_str(Type type){
		switch (type.get_type()) {
		case TypeType::U32: return "unsigned int";
		case TypeType::S32: return "int";
		}
	}

	void CGenerator::emit(std::string code) {
		ss << code;
	}

	void CGenerator::gen() {
		gen(ast);
		spdlog::info(ss.str());
	}

	void CGenerator::gen(AST* ast) {
		switch (ast->get_type()) {
		case AST::Type::PROGRAM: gen(ast->as_program()); break;
		case AST::Type::DECL: gen(ast->as_decl()); break;
		case AST::Type::NUM: gen(ast->as_num()); break;
		}
	}
	void CGenerator::gen(ProgramAST& program) {
		for (auto& ast : program.get_body()) {
			gen(ast);
		}
	}

	void CGenerator::gen(DeclAST& ast) {
		emit(type_to_str(ast.get_type()));
		emit(" ");
		emit(ast.get_token()->get_value());
		if (ast.get_value().has_value()) {
			emit(" = ");
			gen(ast.get_value().value());
		}
	}

	void CGenerator::gen(BlockAST&) {
		emit("{\n");
		emit("}\n");
	}
	void CGenerator::gen(FnAST& fn_ast) {
		emit("{\n");
		emit("}\n");
	}
	void CGenerator::gen(NumAST& ast) {
		emit(ast.get_token()->get_value());
	}
}