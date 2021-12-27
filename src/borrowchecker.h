#include <ast.h>

namespace trove {
	class BorrowChecker {
	public:
		void check(AST*);
		void check(ProgramAST*);
	private:
	};
}