#include "lib/infixlexer.h"
#include "lib/token.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

int main() {

    InfixLexer lexer(std::cin);

    try {

        std::vector<Token> tokens = lexer.infixtokenize();

    for (const Token& token : tokens) {
            std::cout << std::setw(4) << std::right << token.line 
            << std::setw(5) << std::right << token.column << "  "
            << std::left << token.text << std::endl;
        }
    }
    catch (const std::runtime_error& error) {
        // Handle syntax errors
        std::cout << error.what() << std::endl;
        return 1;
    }
    return 0;
}
