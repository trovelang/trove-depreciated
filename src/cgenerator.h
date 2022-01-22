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
        CGenerator(AST* ast) : ast(ast) {}
        std::string type_to_str(Type type);
        void emit(std::string code);
        void gen();
        void gen(CGeneratorContext&, AST*);
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
        void gen_bool(CGeneratorContext&, AST*);
    private:
        AST* ast;
        std::stringstream ss;
    };

}