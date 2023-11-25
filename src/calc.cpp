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
                    std::vector<std::string> results = parser.evaluateToArray(root);
                    if (results.empty()) continue;
                    if (results.size() == 1) std::cout << results[0] << std::endl;
                    else {
                        std::cout << "[";
                        int size = (int)results.size();
                        int count = 0;
                        for (auto result : results) {
                            std::cout << result;
                            if (++count < size) std::cout << ", ";
                            else std::cout << "]\n";
                        }
                    }
                } catch (const std::runtime_error& e) {
                    std::cout << e.what() << std::endl;
                }
            } 
        } catch (const UnexpectedTokenException& e) {
            std::cout << e.what() << std::endl;
        } catch (const SyntaxError& e) {
            std::cout << e.what() << std::endl;
        }
    }
    return 0;
}
