#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/infixParser.h"
#include <sstream>

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

            int openParenthesesCount = 0;  // Track open parentheses
            for (const Token& token : tokens) {
                if (token.type == TokenType::LEFT_PAREN) {
                    openParenthesesCount++;
                } else if (token.type == TokenType::RIGHT_PAREN) {
                    openParenthesesCount--;
                    if (openParenthesesCount < 0) {
                        throw UnexpectedTokenException(")", lexer.line, lexer.column);
                    }
                }
            }

            if (openParenthesesCount > 0) {
                throw UnexpectedTokenException("END", lexer.line, lexer.column+1);
            }

            infixParser parser(tokens, symbolTable);
            ASTNode* root = parser.infixparse();

            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;
                try {
                    std::map<std::string, double> temp = symbolTable;
                    std::cout << root->evaluate(temp) << std::endl;
                    symbolTable = temp;
                    // double result = root->evaluate(symbolTable);
                    // std::cout << result << std::endl;
                } catch (const DivisionByZeroException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const InvalidOperatorException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const UnknownIdentifierException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const SyntaxError& e) {
                    std::cout << e.what() << std::endl;
                }

                delete root;
            } else {
                std::cout << "Failed to parse the input expression." << std::endl;
            }
        } catch (const UnexpectedTokenException& e) {
            std::cout << e.what() << std::endl;
        } catch (const SyntaxError& e) {
            std::cout << e.what() << std::endl;
        }
    }

    return 0;
}