#include <typecheck.h>

namespace trove {

    void TypeCheckPass::analyse() {
        auto ctx = AnalysisCtx{ AnalysisCtx::Scope::GLOBAL };
        analyse(ctx, m_ast);
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx& ctx, AST* ast) {
        switch (ast->get_type()) {
        case AST::Type::PROGRAM: return analyse_program_ast(ctx, ast);
        case AST::Type::BLOCK: return analyse_block_ast(ctx, ast);
        case AST::Type::STATEMENT: return analyse_statement(ctx, ast);
        case AST::Type::DECL: return analyse_decl_ast(ctx, ast);
        case AST::Type::ASSIGN: return analyse_assign_ast(ctx, ast);
        case AST::Type::BIN: return analyse_bin(ctx, ast);
        case AST::Type::NUM: return analyse(ctx, ast->as_num());
        case AST::Type::VAR: return analyse_var(ctx, ast);
        case AST::Type::LOOP: return analyse_loop(ctx, ast);
        case AST::Type::FN: return analyse(ctx, ast->as_fn());
        case AST::Type::STRUCT_DEF: return analyse_struct_def(ctx, ast);
        case AST::Type::STRUCT_LITERAL: return analyse_struct_literal(ctx, ast);
        }
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_statement(AnalysisCtx& ctx, AST* ast) {
        analyse(ctx, ast->as_statement().body);
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_decl_ast(AnalysisCtx& ctx, AST* ast){


        DeclAST& decl = ast->as_decl();
        Type* type;
        AnalysisUnit value_analysis_unit;

        if (decl.value.has_value()) {
            value_analysis_unit = analyse(ctx, decl.value.value());
            
            if (!decl.type.value().complete) {
                decl.type.value() = *value_analysis_unit.type;
                spdlog::info("infering type! {}", decl.type.value().to_string());
            }


            // if we are dealing with a function we need to process it
            if (decl.type->mutability==Type::Mutability::CONST
            && value_analysis_unit.type->base_type == Type::BaseType::FN) {
                // set the global fn name
                value_analysis_unit.type->associated_token = decl.token;
            }
            // if we are dealing with a lambda we need to process it
            else if (decl.type->mutability == Type::Mutability::MUT
                    && value_analysis_unit.type->base_type == Type::BaseType::FN) {
                // set the global fn name
                auto token = new Token(TokenBuilder::builder()
                    .type(Token::Type::IDENTIFIER)
                    .value("lambda")
                    .build());
                value_analysis_unit.type->associated_token = token;
                decl.type.value().associated_token = token;
            }

            // if we are dealing with a function we need to process it
            if (decl.type->mutability == Type::Mutability::CONST
                && value_analysis_unit.type->base_type == Type::BaseType::TYPE) {
                // set the global fn name
                value_analysis_unit.type->associated_token = decl.token;
            }
            // if we are dealing with a lambda we need to process it
            else if (decl.type->mutability == Type::Mutability::MUT
                && value_analysis_unit.type->base_type == Type::BaseType::TYPE) {
                // set the global fn name
                auto token = new Token(TokenBuilder::builder()
                    .type(Token::Type::IDENTIFIER)
                    .value("lambda_type")
                    .build());
                value_analysis_unit.type->associated_token = token;
                decl.type.value().associated_token = token;
            }
        }

        if(!decl.type.has_value()){
            type = value_analysis_unit.type;
        }
        else {
            type = &decl.type.value();
        }

        m_symtable.place(decl.token->value, &ast->as_decl().type.value());
        return AnalysisUnit{ &ast->as_decl().type.value() };
    }

    AnalysisUnit TypeCheckPass::analyse_block_ast(AnalysisCtx& ctx, AST* ast) {
        for (auto& elem : ast->as_block().body) {
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
            m_error_reporter.compile_error(ss.str(), ast->source_position);
        }

        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_program_ast(AnalysisCtx& ctx, AST* ast){
        for (auto& elem : ast->as_program().body) {
            analyse(ctx, elem);
        }
        return {};
    }
    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx& ctx, BlockAST& ast){
        m_symtable.enter();
        for (auto& elem : ast.body) {
            analyse(ctx, elem);
        }
        m_symtable.exit();
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_bin(AnalysisCtx& ctx, AST* ast){
        auto bin = ast->as_bin();
        auto lhs_type = analyse(ctx, bin.lhs);
        auto rhs_type = analyse(ctx, bin.rhs);
        return lhs_type;
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx& ctx, FnAST& ast){
        analyse(ctx, ast.body);
        return AnalysisUnit{&ast.type};
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx&, NumAST& num){
        return AnalysisUnit{&num.type};
    }

    AnalysisUnit TypeCheckPass::analyse_var(AnalysisCtx& ctx, AST* ast){
        auto var = ast->as_var();
        auto type = m_symtable.lookup(var.token->value);
        if (!type.has_value()) {
            m_error_reporter.compile_error("unknown variable", ast->source_position);
        }
        spdlog::info("analysing var {}", type.value()->base_type);
        return AnalysisUnit{ type.value() };
    }

    AnalysisUnit TypeCheckPass::analyse_loop(AnalysisCtx& ctx, AST* ast) {
       
        return AnalysisUnit{  };
    }

    AnalysisUnit TypeCheckPass::analyse_struct_def(AnalysisCtx& ctx, AST* ast) {

        auto struct_def = ast->as_struct_def();
        for (auto& member : struct_def.member_decls) {
            auto member_analysis = analyse(ctx, member);
            struct_def.type.contained_types.push_back(*member_analysis.type);

        }

        return AnalysisUnit{ &ast->as_struct_def().type };
    }

    AnalysisUnit TypeCheckPass::analyse_struct_literal(AnalysisCtx& ctx, AST* ast) {
        for (auto& member : ast->as_struct_literal().member_values) {
            auto member_analysis = analyse(ctx, member);
            ast->as_struct_literal().type.contained_types.push_back(*member_analysis.type);
        }
        return AnalysisUnit{ &ast->as_struct_literal().type };
    }
}