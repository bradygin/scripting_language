#include "lexer.h"
#include "token.h"
#include <cctype>
#include <stdexcept>

Lexer::Lexer(std::istream& input) : sExpression(input) {}

Token Lexer::nextToken() {
    char currChar;
    while (sExpression.get(currChar)) {
        if (currChar == '\n') {
            line++;
            column = 1;
        }else{
            column++;
        }
        if (std::isspace(currChar)) {
                continue; // Skip whitespace
        // LEFT PAREN TOKEN
        } else if (currChar == '(') {
            return Token(line, column, "(", TokenType::LEFT_PAREN);
        // RIGHT PAREN TOKEN
        } else if (currChar == ')') {
            return Token(line, column, ")", TokenType::RIGHT_PAREN);
        // OPERATOR TOKEN
        } else if (currChar == '+' || currChar == '-' || currChar == '*' || currChar == '/') {
            return Token(line, column, std::string(1, currChar), TokenType::OPERATOR);
        // NUMBER TOKEN
        } else if (std::isdigit(currChar)) {
            std::string num;
            num += currChar;
            /*
            Input: 2.14
            Start with 2, add to num

            if (currChar + 1 == isDigit) {
            num += nextChar
            else if (currChar + 1 == ".") {
                
            }

            Now: 2.

            }
            */ 
            return Token(line, column - num.length(), num, TokenType::NUMBER);
        // SYNTAX ERROR, CAN'T CREATE TOKEN
        } else {
            throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
        }
    }
    // NO LONGER A NEXT CHARACTER SO CREATE SPECIAL END TOKEN
    return Token(line, column, "END", TokenType::OPERATOR);
}

std::vector<Token> Lexer::tokenize() {
    Token currToken = nextToken();
 
    while (currToken.text != "END") {
        myTokens.push_back(currToken);
        currToken = nextToken();
    }
    return myTokens;
}
