#include <iostream>
#include <fstream>
#include <iomanip>

#include "lexer.h"
#include "token.h"
#include "infixParser.h"

int main() {
    std::map<std::string, double> symbolTable; // Create the symbol table

    while (true) {
        // Read input
        std::string inputLine;
        if (!std::getline(std::cin, inputLine)) {
            break;
        }
        std::istringstream inputStream(inputLine);
        Lexer lexer(inputStream);
        // Lexer lexer(std::cin);

        try {
            // Tokenize and parse the current line
            std::vector<Token> tokens = lexer.tokenize();
            Parser parser(tokens, symbolTable); // Pass symbolTable to Parser

            ASTNode* root = parser.parse();

            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;

                // Evaluate the expression
                double result = root->evaluate(symbolTable);
                std::cout << result << std::endl;
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
