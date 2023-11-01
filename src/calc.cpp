#include <iostream>
#include <fstream>
#include <iomanip>
#include "lib/infixlexer.h"
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
        InfixLexer lexer(inputStream);

        try {
            // Tokenize and parse the current line
            std::vector<Token> tokens = lexer.infixtokenize();
            infixParser parser(tokens, symbolTable);

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
                throw UnexpectedTokenException("(", lexer.line, lexer.column);
            }

            //infixParser parser(tokens, symbolTable);
            ASTNode* root = parser.infixparse();

            if (root) {
                // Print the AST in infix notation
                std::string infixExpression = parser.printInfix(root);
                std::cout << infixExpression << std::endl;
                try {
                    std::map<std::string, double> temp = symbolTable;
                    std::cout << root->evaluate(temp) << std::endl;
                    symbolTable = temp;
                
                } catch (const DivisionByZeroException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const InvalidOperatorException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const UnknownIdentifierException& e) {
                    std::cout << e.what() << std::endl;
                } catch (const SyntaxError& e) {
                    std::cout << e.what() << std::endl;
                }catch (const InvalidOperandTypeException& e) {
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
