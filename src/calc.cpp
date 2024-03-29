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
    std::map<std::string, double> symbolTable; // Create the symbol table

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

            infixParser parser(tokens, symbolTable);
            ASTNode* root = parser.infixparse();

            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;
                try {
                    std::map<std::string, double> temp = symbolTable;
                    double result = root->evaluate(temp);
                    symbolTable = temp;
                // Check for assignment that evaluates to a boolean value
                Assignment* assignmentNode = dynamic_cast<Assignment*>(root);
                if (assignmentNode && (result == 1.0 || result == 0.0)) {
                    if (result == 1.0) {
                        std::cout << "true" << std::endl;
                    } else {
                        std::cout << "false" << std::endl;
                    }
                } else if (dynamic_cast<BooleanNode*>(root) || (dynamic_cast<Variable*>(root) && (result == 1.0 || result == 0.0)) || (dynamic_cast<BinaryOperation*>(root) && (
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