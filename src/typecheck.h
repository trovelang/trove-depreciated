#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>
#include <symtable.h>
#include <error.h>

namespace trove {

    struct AnalysisCtx {
        enum class Scope {
            GLOBAL,
            LOCAL
        };

        Scope scope;
    };

    struct AnalysisUnit {
        Type* type;
    };

    class TypeCheckPass {
    public:
        TypeCheckPass(ErrorReporter& err_reporter, AST* ast) : err_reporter(err_reporter), ast(ast) {}
        void analyse();
        AnalysisUnit analyse(AnalysisCtx&, AST*);
        AnalysisUnit analyse_decl_ast(AnalysisCtx&, AST*);
        AnalysisUnit analyse_block_ast(AnalysisCtx&, AST*);
        AnalysisUnit analyse_assign_ast(AnalysisCtx&, AST*);
        AnalysisUnit analyse_program_ast(AnalysisCtx&, AST*);
        AnalysisUnit analyse_var(AnalysisCtx&, AST*);
        AnalysisUnit analyse(AnalysisCtx&, BlockAST&);
        AnalysisUnit analyse_bin(AnalysisCtx&, AST*);
        AnalysisUnit analyse(AnalysisCtx&, FnAST&);
        AnalysisUnit analyse(AnalysisCtx&, NumAST&);
    private:
        ErrorReporter& err_reporter;
        AST* ast;
        SymTable<Type*> sym_table;
    };

}