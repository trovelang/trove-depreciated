#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>
#include <vector>
#include <symtable.h>

namespace trove {


    struct ModuleCtx {
        std::vector<std::string*> module_names;
    };

    struct CGeneratorContext {
        u1 in_global;
        ModuleCtx module_ctx;
    };

    class CGenerator {
    public:
        std::string execute(const char* cmd);
        CGenerator(AST* ast) : m_ast(ast) {}
        std::string type_to_str(Type type);
        void emit_raw(std::string code);
        void emit_c_str(std::string str);
        void gen_runtime();
        void gen();
        void gen(CGeneratorContext&, AST*);
        void gen_statement(CGeneratorContext&, AST*);
        void gen(CGeneratorContext&, DeclAST&);
        void gen(CGeneratorContext&, AssignAST&);
        void gen(CGeneratorContext&, ProgramAST&);
        void gen(CGeneratorContext&, BlockAST&);
        void gen_un(CGeneratorContext&, AST*);
        void gen_bin(CGeneratorContext&, AST*);
        void gen_ret(CGeneratorContext&, AST*);
        void gen(CGeneratorContext&, FnAST&);
        void gen(CGeneratorContext&, NumAST&);
        void gen(CGeneratorContext&, VarAST&);
        void gen_call(CGeneratorContext&, AST*);
        void gen_string(CGeneratorContext&, AST*);
        void gen_loop(CGeneratorContext&, AST*);
        void gen_if(CGeneratorContext&, AST*);
        void gen_bool(CGeneratorContext&, AST*);
        void gen_struct_def(CGeneratorContext&, AST*);
        void gen_struct_literal(CGeneratorContext&, AST*);
        void gen_struct_access(CGeneratorContext&, AST*);
        void gen_initialiser_list(CGeneratorContext&, AST*);
    private:
        AST* m_ast;
        SymTable<std::string> m_symtable;
        std::stringstream m_output_stream;
    };

}