#pragma once

#include <ast.h>
#include <type.h>
#include <string>
#include <sstream>

namespace trove {

    class CGenerator {
    public:
        std::string execute(const char* cmd);
        CGenerator(AST* ast) : ast(ast) {}
        std::string type_to_str(Type type);
        void emit(std::string code);
        void gen();
        void gen(AST*);
        void gen(DeclAST&);
        void gen(AssignAST&);
        void gen(ProgramAST&);
        void gen(BlockAST&);
        void gen(BinAST&);
        void gen(FnAST&);
        void gen(NumAST&);
        void gen(VarAST&);
    private:
        AST* ast;
        std::stringstream ss;
    };

}