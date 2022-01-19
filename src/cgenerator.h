#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>

namespace trove {

    class CGenerator {
    public:
        CGenerator(AST* ast) : ast(ast) {}
        std::string type_to_str(Type type);
        void emit(std::string code);
        void gen();
        void gen(AST*);
        void gen(DeclAST&);
        void gen(ProgramAST&);
        void gen(BlockAST&);
        void gen(FnAST&);
        void gen(NumAST&);
    private:
        AST* ast;
        std::stringstream ss;
    };

}