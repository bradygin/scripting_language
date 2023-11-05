#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/infixParser.h"

int main() {
    std::map<std::string, double> symbolTable; // Create the symbol table
    Lexer lexer(std::cin);
    
    try {
        std::vector<Token> tokens = lexer.tokenize();
        infixParser parser(tokens, symbolTable);
        std::vector<ASTNode*> asts = parser.infixparse();
        if (!asts.empty()) {
            try {
                double result = parser.evaluate(symbolTable);
                std::cout << result << std::endl;
            } catch (const DivisionByZeroException& e) {
                std::cout << e.what() << std::endl;
            } catch (const InvalidOperatorException& e) {
                std::cout << e.what() << std::endl;
            } catch (const UnknownIdentifierException& e) {
                std::cout << e.what() << std::endl;
            } catch (const SyntaxError& e) {
                std::cout << e.what() << std::endl;
            }
            delete root;
        }
    } catch (const UnexpectedTokenException& e) {
        std::cout << e.what() << std::endl;
    } catch (const SyntaxError& e) {
        std::cout << e.what() << std::endl;
    }
    return 0;
}
