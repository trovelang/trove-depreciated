#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>
#include <symtable.h>
#include <error.h>

namespace trove {

    struct BorrowCheckCtx {

    };

    struct BorrowCheckUnit {};

    class BorrowCheckPass {
    public:
        BorrowCheckPass(ErrorReporter& err_reporter, AST* ast) : err_reporter(err_reporter), ast(ast) {}
        void analyse();
        BorrowCheckUnit analyse(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse_decl_ast(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse_block_ast(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse_assign_ast(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse_program_ast(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse_var(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse(BorrowCheckCtx&, BlockAST&);
        BorrowCheckUnit analyse_bin(BorrowCheckCtx&, AST*);
        BorrowCheckUnit analyse(BorrowCheckCtx&, FnAST&);
        BorrowCheckUnit analyse(BorrowCheckCtx&, NumAST&);
    private:
        ErrorReporter& err_reporter;
        AST* ast;
        SymTable<Type*> sym_table;
    };

}