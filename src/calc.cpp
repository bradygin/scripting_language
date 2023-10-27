#include <iostream>
#include <fstream>
#include <iomanip>

#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/infixParser.h"

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

        try {
            // Tokenize and parse the current line
            std::vector<Token> tokens = lexer.tokenize();
            Parser infixparser(tokens, symbolTable);

            ASTNode* root = parser.infixparse();

            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;

                // Evaluate the expression and catch custom exceptions
                try {
                    double result = root->evaluate(symbolTable);
                    std::cout << result << std::endl;
                } catch (const DivisionByZeroException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const InvalidOperatorException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const UnknownIdentifierException& e) {
                    std::cout << e.what() << std::endl;
                }

                delete root;
            } else {
                std::cerr << "Failed to parse the input expression." << std::endl;
            }
        } catch (const UnexpectedTokenException& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}