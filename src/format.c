#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/infixParser.h"

int main() {
    Lexer lexer(std::cin);
    
    try {
        std::vector<Token> tokens = lexer.tokenize();
        infixParser parser(tokens);
        while (auto root = parser.infixparse()) {
            // Print the AST in infix notation
            std::string infixExpression = parser.printInfix(root);
            std::cout << infixExpression << std::endl;
        }
    } catch (const SyntaxError& error) {
        std::cout << error.what() << std::endl;
        return 1;
    }catch (const UnexpectedTokenException& e) {
        std::cout << e.what() << std::endl;
        return 2;
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}