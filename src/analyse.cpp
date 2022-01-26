/*

#include <analyse.h>

namespace trove {

    void Analyser::analyse(){
        auto ctx = AnalysisCtx{ AnalysisCtx::Scope::GLOBAL };
        analyse(ast);
    }

    AnalysisUnit Analyser::analyse(AST* ast){
        switch (ast->get_type()) {
        case AST::Type::PROGRAM: return analyse(ast->as_program());
        case AST::Type::DECL: return analyse(ast->as_decl());
        case AST::Type::ASSIGN: return analyse(ast->as_assign());
        case AST::Type::BIN: return analyse(ast->as_bin());
        case AST::Type::NUM: return analyse(ast->as_num());
        case AST::Type::VAR: return analyse(ast->as_var());
        case AST::Type::FN: return analyse(ast->as_fn());
        }
        return {};
    }
    
    AnalysisUnit Analyser::analyse(DeclAST& ast){
        

        Type* type;
        AnalysisUnit value_analysis_unit;

        if (ast.get_value().has_value()) {
            value_analysis_unit = analyse(ast.get_value().value());

            // if we are dealing with a function we need to process it
            if (ast.get_type()->get_mutability_modifier()==MutabilityModifier::CONST
            && value_analysis_unit.type->get_type() == TypeType::FN) {

                // set the global fn name
                value_analysis_unit.type->get_token() = ast.get_token();

            }
        }

        if(!ast.get_type().has_value()){
            type = value_analysis_unit.type;
        }
        else {
            type = &ast.get_type().value();
        }
       

        sym_table.place(ast.get_token()->get_value(), *type);
        return {};
    }
    
    AnalysisUnit Analyser::analyse(AnalysisCtx&, AssignAST&){
        return {};
    }
    AnalysisUnit Analyser::analyse(ProgramAST& ast){
        for (auto& elem : ast.get_body()) {
            analyse(elem);
        }
        return {};
    }
    AnalysisUnit Analyser::analyse(BlockAST& ast){
        sym_table.enter();
        for (auto& elem : ast.get_body()) {
            analyse(elem);
        }
        sym_table.exit();
        return {};
    }
    AnalysisUnit Analyser::analyse(AnalysisCtx&, BinAST&){
        return {};
    }
    
    AnalysisUnit Analyser::analyse(FnAST& ast){
        spdlog::info("analysing fn!");
        return AnalysisUnit{&ast.get_type()};
    }
    
    AnalysisUnit Analyser::analyse(AnalysisCtx&, NumAST& num){
        return {};
    }
    AnalysisUnit Analyser::analyse(AnalysisCtx&, VarAST&){
        return {};
    }
}*/