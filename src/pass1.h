#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>
#include <symtable.h>
#include <error.h>
#include <stack>

#define SAME_CTX() m_analysis_context.top()
#define CTX_REQUIRED_TYPE(ctx, type) ctx.required_type = type;

namespace trove {

    struct AnalysisCtx {
        
        enum class Scope {
            GLOBAL,
            LOCAL
        };

        Scope scope;
        u1 r_value; // e.g. x = 123, the 123 would be an r_value
        u1 l_value;
        std::optional<Type*> required_type;
    };

    struct AnalysisUnit {
        Type* type;
    };

    class CompilationUnit;

    // FIXME we should heap allocate the symbol table otherwise we are doing copies
    struct Pass1Result {
		AST* ast;
		SymTable<Type*> sym_table;
	};

    class Pass1 {
    public:
        Pass1(CompilationUnit* compilation_unit, AST*& ast) : m_compilation_unit(compilation_unit), m_ast(ast) {}
        void register_builtins();
        Pass1Result analyse();
        AnalysisUnit analyse(AnalysisCtx, AST*);
        AnalysisUnit analyse_statement(AST*);
        AnalysisUnit analyse_decl_ast(AST*);
        AnalysisUnit analyse_block(AST*);
        AnalysisUnit analyse_assign_ast(AST*);
        AnalysisUnit analyse_program_ast(AST*);
        AnalysisUnit analyse_var(AST*);
        AnalysisUnit analyse_ret(AST*);
        AnalysisUnit analyse_bool(AST*);
        AnalysisUnit analyse_call(AST*);
        AnalysisUnit analyse_fn(AST*);
        AnalysisUnit analyse(BlockAST&);
        AnalysisUnit analyse_un(AST*);
        AnalysisUnit analyse_bin(AST*);
        AnalysisUnit analyse_loop(AST*);
        AnalysisUnit analyse_struct_def(AST*);
        AnalysisUnit analyse_mod_def(AST*);
        AnalysisUnit analyse_struct_literal(AST*);
        AnalysisUnit analyse_string(AST*);
        AnalysisUnit analyse(FnAST&);
        AnalysisUnit analyse(NumAST&);
        AnalysisUnit analyse_initialiser_list(AST*);

    private:
        CompilationUnit* m_compilation_unit;
        AST*& m_ast;
        SymTable<Type*> m_symtable;
        std::stack<AnalysisCtx> m_analysis_context;
        u32 lambda_count{ 0 };
    };

}