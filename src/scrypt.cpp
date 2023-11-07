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
        double result = 0.0;
        for (ASTNode* root : asts) {
            if (root) {
                result = parser.evaluate(root, symbolTable);
                delete root;
            } else {
                std::cout << "Failed to parse the input expression." << std::endl;
            }
        }
        std::cout << result << std::endl;
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}