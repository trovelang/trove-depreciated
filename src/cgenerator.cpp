#include <cgenerator.h>

namespace trove {

	void CGenerator::gen() {

	}

	void CGenerator::gen(AST* ast) {
		switch (ast->get_type()) {
		case AST::Type::PROGRAM: gen(ast->as_program());
		}
	}
	void CGenerator::gen(ProgramAST& program) {
		for (auto& ast : program.get_body()) {
			gen(ast);
		}
	}
	void CGenerator::gen(BlockAST&) {
	}
	void CGenerator::gen(FnAST&) {}
}