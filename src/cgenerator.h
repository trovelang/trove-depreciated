#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>

namespace trove {

    struct CGeneratorContext {

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
        void gen(CGeneratorContext&, BinAST&);
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
    private:
        AST* m_ast;
        std::stringstream m_output_stream;
    };

}