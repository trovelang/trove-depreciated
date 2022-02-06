#include <unit.h>
#include <pass1.h>
#include <cgenerator.h>
#include <type.h>

namespace trove{

std::vector<Token>* CompilationUnit::lex(){
	auto lexer = Lexer(this, m_source);
	m_tokens = lexer.lex();
    
    return m_tokens;
}

ParseResult CompilationUnit::parse(){
    auto parser = Parser(this, m_tokens);
	m_parse_result = {parser.parse()};
    return m_parse_result;
}

Pass1Result CompilationUnit::pass1(){
    auto pass1 = Pass1(this, m_parse_result.ast);
	m_pass1_result = pass1.analyse();
    return m_pass1_result;
}

ParseResult CompilationUnit::up_to_parse(){
    m_tokens = lex();
    m_parse_result = parse();
    return m_parse_result;
}

Pass1Result CompilationUnit::up_to_pass1(){
    up_to_parse();
    m_pass1_result = pass1();
    return m_pass1_result;
}

void CompilationUnit::compile(){
    auto cgenerator = CGenerator(m_pass1_result.ast);
    cgenerator.gen();
}

void CompilationUnit::up_to_compile(){
    up_to_pass1();
    compile();
}
}