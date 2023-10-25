#include <iostream>
#include <fstream>
#include <iomanip>

#include "lexer.h"  
#include "token.h"  
#include "infixParser.h" 

int main() {
    // Initialize the lexer with standard input (cin)
    Lexer lexer(std::cin);

    try {
        std::vector<Token> tokens = lexer.tokenize();
   
    Parser parser(tokens);
    ASTNode* root = parser.parse();

    if (root) {
        // Print the AST in infix notation
        std::string infixExpression = parser.printInfix(root);
        std::cout << infixExpression << std::endl;

        // Evaluate the expression
        double result = root->evaluate();
        std::cout << result << std::endl;
    } else {
        std::cerr << "Failed to parse the input expression." << std::endl;
    }

    } catch (const std::runtime_error& error) {
        // Handle syntax errors
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}