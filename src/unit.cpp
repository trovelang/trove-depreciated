#include <unit.h>
#include <pass1.h>
#include <cgenerator.h>

namespace trove{

std::vector<Token>* CompilationUnit::lex(){
	auto lexer = Lexer(this, m_source);
	m_tokens = lexer.lex();
    
    return m_tokens;
}

AST* CompilationUnit::parse(){
    auto parser = Parser(this, m_tokens);
	m_ast = parser.parse();
    return m_ast;
}

AST* CompilationUnit::pass1(){
    auto pass1 = Pass1(this, m_ast);
	m_ast = pass1.analyse();
    return m_ast;
}

AST* CompilationUnit::up_to_parse(){
    m_tokens = lex();
    m_ast = parse();
    return m_ast;
}

AST* CompilationUnit::up_to_pass1(){
    up_to_parse();
    m_ast = pass1();
    return m_ast;
}

void CompilationUnit::compile(){
    auto cgenerator = CGenerator(m_ast);
    cgenerator.gen();
}

void CompilationUnit::up_to_compile(){
    up_to_pass1();
    compile();
}
}