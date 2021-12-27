#pragma once

#include <ast.h>

namespace trove {

    class CGenerator {
    public:
        CGenerator(AST* ast) : ast(ast) {}
        void gen();
        void gen(AST*);
        void gen(ProgramAST&);
        void gen(BlockAST&);
        void gen(FnAST&);
    private:
        AST* ast;
    };

}