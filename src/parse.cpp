#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/parser.h"

int main() {
    // Read and tokenize the entire standard input in one pass
    InfixLexer lexer(std::cin);

    try {
        std::vector<Token> tokens = lexer.infixtokenize();

        // Parse the tokens into a sequence of ASTs
        Parser parser(tokens);
        std::vector<Node*> asts = parser.parse();

        for (Node* root : asts) {
            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;

                // Evaluate the expression
                try {
                    double result = root->evaluate();
                    std::cout << result << std::endl;
                } catch (const std::runtime_error& error) {
                    std::cerr << error.what() << std::endl;
                    // Deallocate any remaining ASTs before exiting
                    for (Node* node : asts) {
                        delete node;
                    }
                    return 3;
                }
                // Delete the AST after evaluation to prevent memory leak
                delete root;
            } else {
                std::cerr << "Failed to parse one of the input expressions." << std::endl;
            }
        }

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}