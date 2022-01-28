#include <typecheck.h>
#include <trove.h>

/*

We need to make an important descision here, do we check for scnearios BEFORE or AFTER
dealing with an ast

e.g. main const = {}

Do we turn {} into fn {} during DeclAST analysis <-- FOR NOW WE ARE GOING WITH THIS
Or turn {} into fn {} during FnAST analysis given the context

TODO
the context should perhaps be in a stack?

*/
namespace trove {

    void TypeCheckPass::analyse() {
        auto ctx = AnalysisCtx{ AnalysisCtx::Scope::GLOBAL };
        analyse(ctx, m_ast);
    }

    AnalysisUnit TypeCheckPass::analyse(AnalysisCtx ctx, AST* ast) {

        m_analysis_context.push(ctx);

        AnalysisUnit res{};
        switch (ast->get_type()) {
        case AST::Type::PROGRAM: res = analyse_program_ast(ast); break;
        case AST::Type::BLOCK: res = analyse_block(ast); break;
        case AST::Type::STATEMENT: res = analyse_statement(ast); break;
        case AST::Type::DECL: res = analyse_decl_ast(ast); break;
        case AST::Type::ASSIGN: res = analyse_assign_ast(ast); break;
        case AST::Type::UN: res = analyse_un(ast); break;
        case AST::Type::BIN: res = analyse_bin(ast); break;
        case AST::Type::BOOL: res = analyse_bool(ast); break;
        case AST::Type::NUM: res = analyse(ast->as_num()); break;
        case AST::Type::VAR: res = analyse_var(ast); break;
        case AST::Type::LOOP: res = analyse_loop(ast); break;
        case AST::Type::FN: res = analyse_fn(ast); break;
        case AST::Type::STRING: res = analyse_string(ast); break;
        case AST::Type::STRUCT_DEF: res = analyse_struct_def(ast); break;
        case AST::Type::STRUCT_LITERAL: res = analyse_struct_literal(ast); break;
        case AST::Type::INITIALISER_LIST: res = analyse_initialiser_list(ast); break;
        default: UNREACHABLE("uhhh");
        }

        m_analysis_context.pop();

        return res;
    }

    AnalysisUnit TypeCheckPass::analyse_statement(AST* ast) {
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        analyse(new_ctx, ast->as_statement().body);
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_decl_ast(AST* ast){


        DeclAST& decl = ast->as_decl();
        Type* type;
        AnalysisUnit value_analysis_unit;

        IF_VALUE(decl.value){
            
            auto new_ctx = m_analysis_context.top();
            new_ctx.required_type = &ast->as_decl().type.value();

            value_analysis_unit = analyse(new_ctx, decl.value.value());

            decl.type.value().coerce(*value_analysis_unit.type);
            
            if (!decl.type.value().complete) {
                decl.type.value() = *value_analysis_unit.type;
            }

            // if we are dealing with a function we need to process it
            if (decl.type->mutability==Type::Mutability::CONSTANT
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
            if (decl.type->mutability == Type::Mutability::CONSTANT
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

        IF_NO_VALUE(decl.type){
            type = value_analysis_unit.type;
        }else {
            type = &decl.type.value();
        }

        m_symtable.place(decl.token->value, &ast->as_decl().type.value());
        return AnalysisUnit{ &ast->as_decl().type.value() };
    }

    AnalysisUnit TypeCheckPass::analyse_block(AST* ast) {
        // transform ourself into a fn!
        IF_VALUE(m_analysis_context.top().required_type) {
            if (m_analysis_context.top().required_type.value()->base_type == Type::BaseType::FN) {
                
                for (auto& elem : ast->as_block().body) {
                    auto new_ctx = m_analysis_context.top();
                    new_ctx.scope = AnalysisCtx::Scope::LOCAL;
                    analyse(new_ctx, elem);
                }

                auto fn_body = new AST(*ast);
                auto fn = AST(AST::Type::FN, ast->source_position, FnAST(fn_body, {}, 0, *m_analysis_context.top().required_type.value()));
                *ast = fn;

                return AnalysisUnit{ &ast->as_fn().type };
            }
        }

        for (auto& elem : ast->as_block().body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, elem);
        }
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_assign_ast(AST* ast){

        auto assign = ast->as_assign();

        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto lhs = analyse(new_ctx, assign.assignee);
        auto rhs = analyse(new_ctx, assign.value);


        if (lhs.type->mutability == Type::Mutability::CONSTANT) {
            std::stringstream ss;
            ss << "Attempted to assign to constant " << lhs.type->to_string();
            m_error_reporter.compile_error(ss.str(), ast->source_position);
            return {};
        }

        // see if we can coerce the rhs into the lhs
        lhs.type->coerce(*rhs.type);

        if (!lhs.type->equals(*rhs.type)) {
            std::stringstream ss;
            ss << "types do not equal, expected " << lhs.type->to_string() << " but got " << rhs.type->to_string();
            m_error_reporter.compile_error(ss.str(), ast->source_position);
        }

        return AnalysisUnit{};
    }

    AnalysisUnit TypeCheckPass::analyse_program_ast(AST* ast){
        for (auto& elem : ast->as_program().body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, elem);
        }
        return {};
    }
    AnalysisUnit TypeCheckPass::analyse(BlockAST& ast){
        m_symtable.enter();
        for (auto& elem : ast.body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, elem);
        }
        m_symtable.exit();
        return {};
    }

    AnalysisUnit TypeCheckPass::analyse_un(AST* ast) {
        auto un = ast->as_un();
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto value_type = analyse(new_ctx, un.value);
        return value_type;
    }

    AnalysisUnit TypeCheckPass::analyse_bin(AST* ast){
        auto bin = ast->as_bin();
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto lhs_type = analyse(new_ctx, bin.lhs);
        auto rhs_type = analyse(new_ctx, bin.rhs);
        return lhs_type;
    }

    AnalysisUnit TypeCheckPass::analyse_fn(AST* ast) {
        // todo check if we are a lambda here
        // if we are an r_value
        /*if (ctx.r_value && ctx.parent_type->mutability == Type::Mutability::MUT) {
            ast->as_fn().type.associated_token->value = "lambda_";
            ast->as_fn().type.fn_type.is_lambda = true;
        }
        else if (ctx.r_value && ctx.parent_type->mutability == Type::Mutability::CONSTANT) {
            // get the parents variable name
            auto name = ast->parent->as_decl().token;
            ast->as_fn().type.associated_token->value = "lambda_";
            ast->as_fn().type.fn_type.is_lambda = false;
        }*/

        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        analyse(new_ctx, ast->as_fn().body);
        return AnalysisUnit{ &ast->as_fn().type };
    }

    AnalysisUnit TypeCheckPass::analyse(NumAST& num){
        return AnalysisUnit{&num.type};
    }

    AnalysisUnit TypeCheckPass::analyse_bool(AST* ast) {
        return AnalysisUnit{ &ast->as_bool().type };
    }

    AnalysisUnit TypeCheckPass::analyse_var(AST* ast){
        auto var = ast->as_var();
        auto type = m_symtable.lookup(var.token->value);
        IF_NO_VALUE(type) {
            m_error_reporter.compile_error("unknown variable", ast->source_position);
        }
        return AnalysisUnit{ type.value() };
    }

    AnalysisUnit TypeCheckPass::analyse_loop(AST* ast) {
       
        return AnalysisUnit{  };
    }

    AnalysisUnit TypeCheckPass::analyse_string(AST* ast) {

        return AnalysisUnit{ new Type(TypeBuilder::builder().base_type(Type::BaseType::STRING).build()) };
    }

    AnalysisUnit TypeCheckPass::analyse_struct_def(AST* ast) {

        auto struct_def = ast->as_struct_def();
        for (auto& member : struct_def.member_decls) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            auto member_analysis = analyse(new_ctx, member);
            struct_def.type.contained_types.push_back(*member_analysis.type);

        }

        return AnalysisUnit{ &ast->as_struct_def().type };
    }

    AnalysisUnit TypeCheckPass::analyse_struct_literal(AST* ast) {
        for (auto& member : ast->as_struct_literal().member_values) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            auto member_analysis = analyse(new_ctx, member);
            ast->as_struct_literal().type.contained_types.push_back(*member_analysis.type);
        }
        return AnalysisUnit{ &ast->as_struct_literal().type };
    }

    AnalysisUnit TypeCheckPass::analyse_initialiser_list(AST* ast) {
        auto type = new Type(TypeBuilder::builder().base_type(Type::BaseType::STRUCT).build());
        type->struct_type.is_anonymous = true;
        for (auto& elem : ast->as_initialiser_list().values) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            auto elem_type = analyse(new_ctx, elem);
            type->contained_types.push_back(*elem_type.type);
        }
        return AnalysisUnit{ type };
    }
}