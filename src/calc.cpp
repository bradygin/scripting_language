#include <iostream>
#include <fstream>
#include <iomanip>

#include "lib/lexer.h"  
#include "lib/token.h"  
#include "lib/infixParser.h" 

int main() {
    while (true) {
        // Read one line of input
        std::string inputLine;
        if (!std::getline(std::cin, inputLine)) {
            // End of input
            break;
        }

        // Initialize the lexer with the current line
        std::istringstream inputStream(inputLine);
        Lexer lexer(inputStream);

        try {
            // Tokenize and parse the current line
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
                //delete root;
            } else {
                std::cerr << "Failed to parse the input expression." << std::endl;
            }
        delete root;
        } catch (const std::runtime_error& error) {
            // Handle syntax errors
            std::cerr << error.what() << std::endl;
        }
    }

    return 0;
}