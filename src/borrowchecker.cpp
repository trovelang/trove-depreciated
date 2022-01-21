#include <borrowchecker.h>

namespace trove {

    void BorrowCheckPass::analyse() {
        auto ctx = BorrowCheckCtx{ };
        analyse(ctx, ast);
    }

    BorrowCheckUnit BorrowCheckPass::analyse(BorrowCheckCtx& ctx, AST* ast) {
        switch (ast->get_type()) {
        case AST::Type::PROGRAM: return analyse_program_ast(ctx, ast);
        case AST::Type::BLOCK: return analyse_block_ast(ctx, ast);
        case AST::Type::DECL: return analyse_decl_ast(ctx, ast);
        case AST::Type::ASSIGN: return analyse_assign_ast(ctx, ast);
        case AST::Type::BIN: return analyse_bin(ctx, ast);
        case AST::Type::NUM: return analyse(ctx, ast->as_num());
        case AST::Type::VAR: return analyse_var(ctx, ast);
        case AST::Type::FN: return analyse(ctx, ast->as_fn());
        }
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse_decl_ast(BorrowCheckCtx& ctx, AST* ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse_block_ast(BorrowCheckCtx& ctx, AST* ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse_assign_ast(BorrowCheckCtx& ctx, AST* ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse_program_ast(BorrowCheckCtx& ctx, AST* ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse(BorrowCheckCtx& ctx, BlockAST& ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse_bin(BorrowCheckCtx& ctx, AST* ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse(BorrowCheckCtx& ctx, FnAST& ast) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse(BorrowCheckCtx&, NumAST& num) {
        return {};
    }

    BorrowCheckUnit BorrowCheckPass::analyse_var(BorrowCheckCtx& ctx, AST* ast) {
        return {};
    }
}