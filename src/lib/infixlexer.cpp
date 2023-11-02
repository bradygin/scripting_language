#include "infixlexer.h"
#include "token.h"
#include <cctype>
#include <stdexcept>
#include <cmath>

InfixLexer::InfixLexer(std::istream& input) : sExpression(input) {}

Token InfixLexer::infixnextToken() {
    char currChar;

    while (sExpression >> std::noskipws >> currChar) {
        if (currChar == '\n') {
            line++;
            column = 0;
        } else {
            column++;
        }

        if (std::isspace(currChar)) {
            continue;
        } else if (currChar == '(') {
            return Token(line, column, "(", TokenType::LEFT_PAREN);
        } else if (currChar == ')') {
            return Token(line, column, ")", TokenType::RIGHT_PAREN);
        } else if (currChar == '+'
                   || currChar == '-'
                   || currChar == '*'
                   || currChar == '/'
                   || currChar == '%') {
            return Token(line, column, std::string(1, currChar), TokenType::OPERATOR);
        } else if (currChar == '=') {
            char nextChar = sExpression.peek();
            if (nextChar == '=') {
                sExpression.get();
                column ++;
                return Token(line, column-1, "==", TokenType::OPERATOR);
            } else {
                return Token(line, column, "=", TokenType::ASSIGNMENT);
            }
        } else if (currChar == '<' || currChar == '>') {
            char nextChar = sExpression.peek();
            if (nextChar == '=') {
                sExpression.get();
                return Token(line, column, std::string(1, currChar) + "=", TokenType::OPERATOR);
            } else {
                return Token(line, column, std::string(1, currChar), TokenType::OPERATOR);
            }
        } else if (currChar == '!') {
            char nextChar = sExpression.peek();
            if (nextChar == '=') {
                sExpression.get();
                column ++;
                return Token(line, column-1, "!=", TokenType::OPERATOR);
            } else {
                throw SyntaxError(line, column);
            }
        } else if (currChar == '&') {
            return Token(line, column, "&", TokenType::OPERATOR);
        } else if (currChar == '|') {
            return Token(line, column, "|", TokenType::OPERATOR);
        } else if (currChar == '^') {
            return Token(line, column, "^", TokenType::OPERATOR);
        } else if (currChar == '{') {
            return Token(line, column, "{", TokenType::OPERATOR);
        } else if (currChar == '}') {
            return Token(line, column, "}", TokenType::OPERATOR);
        }else if (std::isdigit(currChar)) {
            std::string num;
            num += currChar;
            char nextChar;

            while (sExpression.get(nextChar)) {
                if (std::isdigit(nextChar) || nextChar == '.') {
                    if (nextChar == '.' && num.find('.') == std::string::npos) {
                        char followingChar = sExpression.peek();
                        if (!std::isdigit(followingChar)) {
                            throw SyntaxError(line, (column + 2));
                        }
                    }
                    column++;
                    if (nextChar == '.' && num.find('.') != std::string::npos) {
                        throw SyntaxError(line, (column));
                    }
                    num += nextChar;
                } else {
                    sExpression.unget();
                    break;
                }
            }

            return Token(line, (column - num.length() + 1), num, TokenType::NUMBER);
        } else if (isalpha(currChar) || currChar == '_') {
            std::string identifier;
            identifier += currChar;
            char nextChar;

            while (sExpression.get(nextChar)) {
                if (isalnum(nextChar) || nextChar == '_') {
                    identifier += nextChar;
                } else {
                    sExpression.unget();
                    break;
                }
            }
            int tempColumn = column;
            column += identifier.length() -1 ;
            if (identifier == "true" || identifier == "false") {
                return Token(line, tempColumn , identifier, TokenType::BOOLEAN);
            } else {
                return Token(line, tempColumn, identifier, TokenType::IDENTIFIER);
            }
        } else {
            throw SyntaxError(line, (column));
        }
    }

    return Token(line, column + 1, "END", TokenType::OPERATOR);
}

std::vector<Token> InfixLexer::infixtokenize() {
    Token currToken = infixnextToken();

    while (currToken.text != "END") {
        myTokens.push_back(currToken);
        currToken = infixnextToken();
    }

    myTokens.push_back(currToken);

    return myTokens;
}