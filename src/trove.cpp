// trove.cpp : Defines the entry point for the application.
//

#include <trove.h>
#include <lexer.h>

using namespace std;

int main()
{


	const auto source = "x : 123";


	auto lexer = Lexer(source);
	
	
	auto tokens = lexer.lex();
	

	for (auto token : tokens) {
		cout << "token = " << token.to_string() << endl;
	}

	return 0;
}
