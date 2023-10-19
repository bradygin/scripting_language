#include <iostream>
#include <fstream>
#include <iomanip>

#include "lexer.h"  
#include "token.h"  
#include "infixParser.h" 

int main() {
    // Open a file containing your input expression
    std::cout << "Enter your input:" << std::endl;

    // Initialize the lexer with standard input (cin)
    Lexer lexer(std::cin);

    try {
        std::vector<Token> tokens = lexer.tokenize();

        // Print the tokens and their line and column numbers
      for (const Token& token : tokens) {
            std::cout << std::setw(4) << token.line << std::setw(4) << token.column << "  "
                      << std::setw(4) << token.text << std::endl;
        }     

    std::cout << "\n";
    Parser parser(tokens);
    ASTNode* root = parser.parse();

    if (root) {
        // Print the AST in infix notation
        std::string infixExpression = parser.printInfix(root);
        std::cout << "Infix Expression: " << infixExpression << std::endl;

        // Evaluate the expression
        double result = root->evaluate();
        std::cout << "Result: " << result << std::endl;
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