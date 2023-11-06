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

        // Create a vector to hold the parsed AST nodes
        std::vector<ASTNode*> asts;

        infixParser parser(tokens, symbolTable);

        // Parse multiple expressions/statements into the AST vector
        while (parser.currentToken.text != "END") {
            ASTNode* ast = parser.infixparse();
            if (ast) {
                asts.push_back(ast);
            } else {
                std::cout << "Failed to parse the input expression." << std::endl;
                break; // Exit the loop if parsing fails
            }
        }

        // Evaluate each AST node in the vector
        double result = 0.0;

        // Evaluate each AST node in the vector
        for (ASTNode* root : asts) {
            result = root->evaluate(symbolTable);
            delete root;
        }

        std::cout << result << std::endl;

    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
