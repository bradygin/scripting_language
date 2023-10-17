#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "lexer.h"
#include "token.h"
#include "parser.h"

int main() {
    std::cout << "Enter your input:" << std::endl;

    // Read input from standard input (cin)
    Lexer lexer(std::cin);

    try {
        // Tokenize the input
        std::vector<Token> tokens = lexer.tokenize();

        // Print the tokens and their line and column numbers
        for (const Token& token : tokens) {
            std::cout << std::setw(4) << token.line << std::setw(4) << token.column << "  "
                      << std::setw(4) << token.text << std::endl;
        }

        // Create a parser and pass the tokens for parsing
        Parser parser(tokens);
        std::cout << "\n" << std::endl;
        Node* root = parser.parse();

        if (root) {
            // Print the AST in infix notation
            std::string infixExpression = parser.printInfix(root);
            std::cout << infixExpression << std::endl;

            //Evaluate the expression
            try {
                double result = root->evaluate();
                std::cout << result << std::endl;
            } catch (const std::runtime_error& error) {
                std::cerr << error.what() << std::endl;
                return 3;
            }
        } else {
            std::cerr << "Failed to parse the input expression." << std::endl;
        }

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}
