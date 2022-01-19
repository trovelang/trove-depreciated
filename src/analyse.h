#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>
#include <symtable.h>

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

    class Analyser {
    public:
        Analyser(AST* ast) : ast(ast) {}
        void analyse();
        AnalysisUnit analyse(AnalysisCtx&, AST*);
        AnalysisUnit analyse(AnalysisCtx&, DeclAST&);
        AnalysisUnit analyse(AnalysisCtx&, AssignAST&);
        AnalysisUnit analyse(AnalysisCtx&, ProgramAST&);
        AnalysisUnit analyse(AnalysisCtx&, BlockAST&);
        AnalysisUnit analyse(AnalysisCtx&, BinAST&);
        AnalysisUnit analyse(AnalysisCtx&, FnAST&);
        AnalysisUnit analyse(AnalysisCtx&, NumAST&);
        AnalysisUnit analyse(AnalysisCtx&, VarAST&);
    private:
        AST* ast;
        SymTable<Type> sym_table;
    };

}