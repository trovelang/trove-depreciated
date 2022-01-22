#include <typecheck.h>

namespace trove {

    void TypeCheckPass::analyse(){
        auto ctx = AnalysisCtx{ AnalysisCtx::Scope::GLOBAL };
        analyse(ctx, ast);
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx& ctx, AST* ast){
        switch (ast->get_type()) {
        case AST::Type::PROGRAM: return analyse_program_ast(ctx, ast);
        case AST::Type::BLOCK: return analyse_block_ast(ctx, ast);
        case AST::Type::DECL: return analyse_decl_ast(ctx, ast);
        case AST::Type::ASSIGN: return analyse_assign_ast(ctx, ast);
        case AST::Type::BIN: return analyse_bin(ctx, ast);
        case AST::Type::NUM: return analyse(ctx, ast->as_num());
        case AST::Type::VAR: return analyse_var(ctx, ast);
        case AST::Type::LOOP: return analyse_loop(ctx, ast);
        case AST::Type::FN: return analyse(ctx, ast->as_fn());
        }
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_decl_ast(AnalysisCtx& ctx, AST* ast){


        DeclAST& decl = ast->as_decl();
        Type* type;
        AnalysisUnit value_analysis_unit;

        if (decl.get_value().has_value()) {
            value_analysis_unit = analyse(ctx, decl.get_value().value());

            // if we are dealing with a function we need to process it
            if (decl.get_type()->get_mutability_modifier()==MutabilityModifier::CONST
            && value_analysis_unit.type->get_type() == TypeType::FN) {

                // set the global fn name
                value_analysis_unit.type->get_token() = decl.get_token();

            }
            // if we are dealing with a lambda we need to process it
            else if (decl.get_type()->get_mutability_modifier() == MutabilityModifier::MUT
                    && value_analysis_unit.type->get_type() == TypeType::FN) {
                
                spdlog::info("analysing lambda!");
                // set the global fn name

                auto tok = new Token(Token::Type::IDENTIFIER, {}, "lambda");
                value_analysis_unit.type->get_token() = tok;

                decl.get_type().value().token = tok;


            }
        }

        if(!decl.get_type().has_value()){
            type = value_analysis_unit.type;
        }
        else {
            type = &decl.get_type().value();
        }


        sym_table.place(decl.get_token()->get_value(), &ast->as_decl().get_type().value());
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_block_ast(AnalysisCtx& ctx, AST* ast) {
        for (auto& elem : ast->as_block().get_body()) {
            analyse(ctx, elem);
        }
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_assign_ast(AnalysisCtx& ctx, AST* ast){

        auto assign = ast->as_assign();

        auto lhs = analyse(ctx, assign.assignee);
        auto rhs = analyse(ctx, assign.value);

        if (!lhs.type->equals(*rhs.type)) {
            std::stringstream ss;
            ss << "types do not equal, expected " << lhs.type->to_string() << " but got " << rhs.type->to_string();
            err_reporter.compile_error(ss.str(), ast->source_position);
        }

        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_program_ast(AnalysisCtx& ctx, AST* ast){
        for (auto& elem : ast->as_program().get_body()) {
            analyse(ctx, elem);
        }
        return {};
    }
    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx& ctx, BlockAST& ast){
        sym_table.enter();
        for (auto& elem : ast.get_body()) {
            analyse(ctx, elem);
        }
        sym_table.exit();
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_bin(AnalysisCtx& ctx, AST* ast){
        auto bin = ast->as_bin();
        auto lhs_type = analyse(ctx, bin.get_lhs());
        auto rhs_type = analyse(ctx, bin.get_rhs());
        return lhs_type;
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx& ctx, FnAST& ast){
        analyse(ctx, ast.get_body());
        return AnalysisUnit{&ast.get_type()};
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx&, NumAST& num){
        return AnalysisUnit{&num.type};
    }

    AnalysisUnit TypeCheckPass::analyse_var(AnalysisCtx& ctx, AST* ast){
        auto var = ast->as_var();
        auto type = sym_table.lookup(var.get_token()->get_value());
        if (!type.has_value()) {
            err_reporter.compile_error("unknown variable", ast->source_position);
        }
        spdlog::info("analysing var {}", type.value()->get_type());
        return AnalysisUnit{ type.value() };
    }

    AnalysisUnit TypeCheckPass::analyse_loop(AnalysisCtx& ctx, AST* ast) {
       
        return AnalysisUnit{  };
    }
}