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
        
        for (ASTNode* root : asts) {
            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;
                delete root;
            } else {
                std::cout << "Failed to parse the input expression." << std::endl;
            }
        }
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
