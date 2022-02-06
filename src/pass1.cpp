#include <pass1.h>
#include <trove.h>
#include <unit.h>

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

    void Pass1::register_builtins(){
        m_symtable.place("printf", new Type(
            TypeBuilder::builder()
            .base_type(Type::BaseType::FN)
            .contained_types({
                TypeBuilder::builder().base_type(Type::BaseType::STRING).build(),
                TypeBuilder::builder().base_type(Type::BaseType::NONE).build(),
                })
            .build()));
    }

    Pass1Result Pass1::analyse() {
        register_builtins();
        auto ctx = AnalysisCtx{ AnalysisCtx::Scope::GLOBAL };
        analyse(ctx, m_ast);
        return Pass1Result{m_ast, m_symtable};
    }

    AnalysisUnit Pass1::analyse(AnalysisCtx ctx, AST* ast) {

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
        case AST::Type::RET: res = analyse_ret(ast); break;
        case AST::Type::NUM: res = analyse(ast->as_num()); break;
        case AST::Type::VAR: res = analyse_var(ast); break;
        case AST::Type::LOOP: res = analyse_loop(ast); break;
        case AST::Type::CALL: res = analyse_call(ast); break;
        case AST::Type::FN: res = analyse_fn(ast); break;
        case AST::Type::STRING: res = analyse_string(ast); break;
        case AST::Type::STRUCT_DEF: res = analyse_struct_def(ast); break;
        case AST::Type::MODULE: res = analyse_mod_def(ast); break;
        case AST::Type::STRUCT_LITERAL: res = analyse_struct_literal(ast); break;
        case AST::Type::INITIALISER_LIST: res = analyse_initialiser_list(ast); break;
        case AST::Type::STRUCT_ACCESS: res = analyse_struct_access(ast); break;
        default: UNREACHABLE("uhhh");
        }

        m_analysis_context.pop();

        return res;
    }

    AnalysisUnit Pass1::analyse_statement(AST* ast) {
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        analyse(new_ctx, ast->as_statement().body);
        return {};
    }

    AnalysisUnit Pass1::analyse_decl_ast(AST* ast){


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


            // FIXME if we have x Vector
            // at this point the Vector type isn't resolved, its only a placeholder
            // we need to look up into the symtable to find out the actual value
            if (decl.type.value().base_type == Type::BaseType::STRUCT) {

                auto struct_type = m_symtable.lookup(decl.type.value().associated_token->value);
                ast->as_decl().type.value() = *struct_type.value();
                ast->as_decl().type.value().base_type = Type::BaseType::STRUCT;
            }


            if (!decl.type->equals(*value_analysis_unit.type)) {
                std::stringstream ss;
                ss << "types do not equal, expected " << decl.type->to_string() << " but got " << value_analysis_unit.type->to_string();
                m_compilation_unit->err_reporter().compile_error(ss.str(), ast->source_position);
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
                    .value(std::string("lambda_").append(std::to_string(lambda_count++)))
                    .build());
                value_analysis_unit.type->associated_token = token;
                decl.type.value().associated_token = token;
            }

            // if we are dealing with a type we need to process it
            if (decl.type->mutability == Type::Mutability::CONSTANT
                && value_analysis_unit.type->base_type == Type::BaseType::TYPE) {
                // set the global fn name
                value_analysis_unit.type->associated_token = decl.token;
            }
            // if we are dealing with a lambda type we need to process it
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

            // if we are dealing with a module we need to process it
            else if (decl.type->mutability == Type::Mutability::CONSTANT
                && value_analysis_unit.type->base_type == Type::BaseType::MODULE) {
                // set the global mod name
                value_analysis_unit.type->associated_token = decl.token;
            }
            if(m_symtable.lookup_only_level(decl.token->value, m_symtable.level()).has_value()){
                if(decl.type.value().base_type==Type::BaseType::FN){

                    // FIXME check for overloading
                    // the problem here is we loose track of the 
                    auto type_mangling = decl.type.value().mangled_to_string();
                    ast->as_decl().token->value.append(type_mangling);

                }else{
                    m_compilation_unit->err_reporter().compile_error("variable already defined", ast->get_position());
                    return {};
                }
            }
            
        }

        IF_NO_VALUE(decl.type){
            type = value_analysis_unit.type;
        }else {
            type = &decl.type.value();
        }

        // if we encounter a function then mangle it
        if(decl.token->value!="main" && decl.type.value().base_type==Type::BaseType::FN){
            ast->as_decl().token->value = Type::mangle_identifier_with_type(ast->as_decl().token->value, decl.type.value());
        }

        m_symtable.place(ast->as_decl().token->value, &ast->as_decl().type.value());
        return AnalysisUnit{ &ast->as_decl().type.value() };
    }

    AnalysisUnit Pass1::analyse_block(AST* ast) {
        // transform ourself into a fn!
        IF_VALUE(m_analysis_context.top().required_type) {
            if (m_analysis_context.top().required_type.value()->base_type == Type::BaseType::FN) {
                
                for (auto& elem : ast->as_block().body) {
                    auto new_ctx = m_analysis_context.top();
                    new_ctx.scope = AnalysisCtx::Scope::LOCAL;
                    analyse(new_ctx, elem);
                }

                auto return_type = TypeBuilder::builder().base_type(Type::BaseType::NONE).build();
                auto fn_type = TypeBuilder::builder().base_type(Type::BaseType::FN).contained_types({ return_type }).build();
                auto fn_body = new AST(*ast);
                auto fn = AST(AST::Type::FN, ast->source_position, FnAST(fn_body, {}, return_type, 0, *m_analysis_context.top().required_type.value()));
                *ast = fn;

                return AnalysisUnit{ &ast->as_fn().type };
            }
            else {
                if (m_analysis_context.top().required_type.value()->base_type == Type::BaseType::TYPE) {

                }
            }
        }

        for (auto& elem : ast->as_block().body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, elem);
        }
        return {};
    }

    AnalysisUnit Pass1::analyse_assign_ast(AST* ast){

        auto assign = ast->as_assign();

        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto lhs = analyse(new_ctx, assign.assignee);
        auto rhs = analyse(new_ctx, assign.value);


        if (lhs.type->mutability == Type::Mutability::CONSTANT) {
            std::stringstream ss;
            ss << "Attempted to assign to constant " << lhs.type->to_string();
            m_compilation_unit->err_reporter().compile_error(ss.str(), ast->source_position);
            return {};
        }

        // see if we can coerce the rhs into the lhs
        lhs.type->coerce(*rhs.type);

        if (!lhs.type->equals(*rhs.type)) {
            std::stringstream ss;
            ss << "types do not equal, expected " << lhs.type->to_string() << " but got " << rhs.type->to_string();
            m_compilation_unit->err_reporter().compile_error(ss.str(), ast->source_position);
        }

        return AnalysisUnit{};
    }

    AnalysisUnit Pass1::analyse_program_ast(AST* ast){
        for (auto& elem : ast->as_program().body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, elem);
        }
        return {};
    }
    AnalysisUnit Pass1::analyse(BlockAST& ast){
        m_symtable.enter();
        for (auto& elem : ast.body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, elem);
        }
        m_symtable.exit();
        return {};
    }

    AnalysisUnit Pass1::analyse_un(AST* ast) {
        auto un = ast->as_un();
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto value_type = analyse(new_ctx, un.value);
        return value_type;
    }

    AnalysisUnit Pass1::analyse_bin(AST* ast){
        auto bin = ast->as_bin();
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto lhs_type = analyse(new_ctx, bin.lhs);
        auto rhs_type = analyse(new_ctx, bin.rhs);
        return lhs_type;
    }

    AnalysisUnit Pass1::analyse_fn(AST* ast) {
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

        for (const auto& param : ast->as_fn().params) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            auto param_type = analyse(new_ctx, param);

            ast->as_fn().type.contained_types.push_back(*param_type.type);
        }

        ast->as_fn().type.contained_types.push_back(ast->as_fn().return_type);

        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        analyse(new_ctx, ast->as_fn().body);
        return AnalysisUnit{ &ast->as_fn().type };
    }

    AnalysisUnit Pass1::analyse(NumAST& num){
        return AnalysisUnit{&num.type};
    }

    AnalysisUnit Pass1::analyse_ret(AST* ast) {
        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        return analyse(new_ctx, ast->as_ret().value);
    }

    AnalysisUnit Pass1::analyse_bool(AST* ast) {
        return AnalysisUnit{ &ast->as_bool().type };
    }

    AnalysisUnit Pass1::analyse_var(AST* ast){
        auto var = ast->as_var();
        auto type = m_symtable.lookup(var.token->value);
        IF_NO_VALUE(type) {
            m_compilation_unit->err_reporter().compile_error("unknown variable", ast->source_position);
        }
        return AnalysisUnit{ type.value() };
    }

    AnalysisUnit Pass1::analyse_loop(AST* ast) {
       
        return AnalysisUnit{  };
    }

    AnalysisUnit Pass1::analyse_call(AST* ast) {

        auto call = ast->as_call();

        if(call.callee->get_type()==AST::Type::VAR){
            auto var_value = ast->as_call().callee->as_var().token->value;
            
            if(var_value=="include"){

                if(ast->as_call().args.size()==0){
                    m_compilation_unit->err_reporter().compile_error("include must have at least 1 argument", ast->source_position);
                    return {};
                }

                auto module = new Type(TypeBuilder::builder().base_type(Type::BaseType::MODULE).build());
                auto include_file = ast->as_call().args[0]->as_str().token->value;
                auto working_dir = m_compilation_unit->working_dir();
                auto module_src = load_file_with_working_dir(working_dir, include_file);

                auto compilation_unit = CompilationUnit(include_file, module_src, m_compilation_unit->working_dir());
                auto pass1_result = compilation_unit.up_to_pass1();

                // merge our sym tables together
                m_symtable.merge(pass1_result.sym_table);

                auto new_ast = new AST(AST::Type::MODULE, ast->source_position, ModuleAST({pass1_result.ast}));
                *ast = *new_ast;

                return AnalysisUnit{module};
            }
        }

        std::vector<Type> param_types;
        for(auto& arg : ast->as_call().args){
            auto new_ctx = SAME_CTX();
            param_types.push_back(*(analyse(new_ctx, arg).type));
        }

        //logger.debug() << "analysing call " << ast->as_call().callee->as_var().token->value << "\n";

        // TODO if its a struct access we need to mangle it

        if (ast->as_call().callee->get_type() == AST::Type::VAR) {
            if (ast->as_call().callee->as_var().token->value != "printf") {
               auto mangled_name = Type::mangle_identifier_with_types(
                    ast->as_call().callee->as_var().token->value.append("_FN"),
                    param_types);
               ast->as_call().callee->as_var().token->value = mangled_name;
            }
        }else if(ast->as_call().callee->get_type()==AST::Type::STRUCT_ACCESS){
            // FIXME this will crash if the member is not a string
            auto member = ast->as_call().callee->as_struct_access().member->as_var().token->value;
            auto mangled_name = Type::mangle_identifier_with_types(
                member.append("_FN"), 
                param_types);
            ast->as_call().callee->as_struct_access().member->as_var().token->value = mangled_name;
        }

        auto new_ctx = SAME_CTX();
        CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
        auto callee_type = analyse(new_ctx, ast->as_call().callee);

        if (callee_type.type->base_type != Type::BaseType::FN) {
            m_compilation_unit->err_reporter().compile_error("callee must be function", ast->source_position);
            return {};
        }
        return AnalysisUnit{ &callee_type.type->contained_types.at(callee_type.type->contained_types.size()-1) };
    }

    AnalysisUnit Pass1::analyse_string(AST* ast) {

        return AnalysisUnit{ new Type(TypeBuilder::builder().base_type(Type::BaseType::STRING).build()) };
    }

    AnalysisUnit Pass1::analyse_struct_def(AST* ast) {

        auto struct_def = ast->as_struct_def();
        for (auto& member : struct_def.member_decls) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            auto member_analysis = analyse(new_ctx, member);
            ast->as_struct_def().type.contained_types.push_back(*member_analysis.type);

        }

        return AnalysisUnit{ &ast->as_struct_def().type };
    }

    AnalysisUnit Pass1::analyse_mod_def(AST* ast) {

        auto module = ast->as_module();
        for (auto& member : module.body) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            analyse(new_ctx, member);
        }

        return AnalysisUnit{ &ast->as_module().type };
    }

    AnalysisUnit Pass1::analyse_struct_literal(AST* ast) {
        for (auto& member : ast->as_struct_literal().member_values) {
            auto new_ctx = SAME_CTX();
            CTX_REQUIRED_TYPE(new_ctx, std::optional<Type*>());
            auto member_analysis = analyse(new_ctx, member);
            ast->as_struct_literal().type.contained_types.push_back(*member_analysis.type);
        }
        return AnalysisUnit{ &ast->as_struct_literal().type };
    }

    AnalysisUnit Pass1::analyse_initialiser_list(AST* ast) {
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

    AnalysisUnit Pass1::analyse_struct_access(AST* ast) {
        // TODO we need to do mangling stuff
        /*
        logger.errr() << "analysing struct access \n obj: " 
            << ast->as_struct_access().obj->to_string() 
            << "\nmember: "<<ast->as_struct_access().member->to_string()<<"\n";

        auto new_ctx = SAME_CTX();
        auto t = analyse(new_ctx, ast->as_struct_access().obj);
        */

        auto new_ctx = SAME_CTX();
        auto obj_type = analyse(new_ctx, ast->as_struct_access().obj);

        // if its a namepsace then lookup the value in the namespace
        if (obj_type.type->base_type == Type::BaseType::MODULE) {


            return analyse(new_ctx, ast->as_struct_access().member);

        }

        return AnalysisUnit{ 0 };
    }
}