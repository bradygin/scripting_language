
#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/infixParser.h"

class TypeError : public std::runtime_error {
public:
    TypeError(const std::string& message) : std::runtime_error(message) {}
};

int main() {

    while (true) {
        // Reads input
        std::string inputLine;
        if (!std::getline(std::cin, inputLine)) {
            break;
        }
        // Below line is debug helper that prints out the input
        // std::cout << "Debug Input: " << inputLine << std::endl;
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

            infixParser parser(tokens);
            auto root = parser.infixparse();

            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;
                try {
                    double result = parser.evaluate(root);
                    // Check for assignment that evaluates to a boolean value
                    auto assignmentNode = std::dynamic_pointer_cast<Assignment>(root);
                    if (assignmentNode && (result == 1.0 || result == 0.0)) {
                        if (result == 1.0) {
                            std::cout << "true" << std::endl;
                        } else {
                            std::cout << "false" << std::endl;
                        }
                    } else if (std::dynamic_pointer_cast<BooleanNode>(root) || 
                        (std::dynamic_pointer_cast<Variable>(root) && (result == 1.0 || result == 0.0)) || 
                        (std::dynamic_pointer_cast<BinaryOperation>(root) && (
                        root->toInfix().find("<") != std::string::npos ||
                        root->toInfix().find(">") != std::string::npos ||
                        root->toInfix().find("==") != std::string::npos ||
                        root->toInfix().find("!=") != std::string::npos ||
                        root->toInfix().find("<=") != std::string::npos ||
                        root->toInfix().find(">=") != std::string::npos ||
                        root->toInfix().find("&") != std::string::npos ||
                        root->toInfix().find("^") != std::string::npos ||
                        root->toInfix().find("|") != std::string::npos))) {
                            if (result == 1.0) {
                                std::cout << "true" << std::endl;
                            } else {
                                std::cout << "false" << std::endl;
                            }
                    } else {
                        std::cout << result << std::endl;
                    }
                } catch (const std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
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