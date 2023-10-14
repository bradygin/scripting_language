#include "lexer.h"
#include "token.h"
#include <cctype>
#include <stdexcept>

// JUST TO CHECK
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

Lexer::Lexer(std::istream& input) : sExpression(input) {}

Token Lexer::nextToken() {
    char currChar;

    if (sExpression.get(currChar)) {
        if (currChar == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }

        if (std::isspace(currChar)) {
            // Skip whitespace
            return nextToken();
        } else if (currChar == '(') {
            return Token(line, column, "(", TokenType::LEFT_PAREN);
        } else if (currChar == ')') {
            return Token(line, column, ")", TokenType::RIGHT_PAREN);
        } else if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/') {
            return Token(line, column, std::string(1, currChar), TokenType::OPERATOR);
        } else if (std::isdigit(currChar)) {
            std::string num;
            num += currChar;
            char nextChar;

            while (sExpression.get(nextChar)) {
                if (std::isdigit(nextChar) || nextChar == '.') {
                    num += nextChar;
                } else {
                    sExpression.unget();
                    break;
                }

                if (std::isdigit(nextChar)) {
                    column++;
                }
            }

            if (num.find('.') != std::string::npos) {
                if (num.front() == '.' || num.back() == '.' || num.find('.') != num.rfind('.')) {
                    throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
                }
            }

            return Token(line, column - num.length() + 1, num, TokenType::NUMBER);
        } else {
            throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
        }
    } else {
        // If you reach the end of the input, create and return the "END" token
        return Token(line, column, "END", TokenType::OPERATOR);
    }
}



std::vector<Token> Lexer::tokenize() {
    Token currToken = nextToken();

    while (sExpression) {  // Check if there are more tokens to read
        myTokens.push_back(currToken);
        std::cout << "Current Token: " << currToken.line << " " << currToken.column << " " << currToken.text << std::endl;
        currToken = nextToken();
    }

    return myTokens;
}


