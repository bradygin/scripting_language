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
        double result = 0.0;
        while (auto root = parser.infixparse()) {              
            result = parser.evaluate(root, symbolTable);
            delete root;
        }
        std::cout << result << std::endl;
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
