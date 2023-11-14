#include "lexer.h"
#include "token.h"
#include <cctype>
#include <stdexcept>
#include <cmath>

Lexer::Lexer(std::istream& input) : sExpression(input) {}

Token Lexer::nextToken() {
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
                column++;
                return Token(line, column - 1, "==", TokenType::OPERATOR);
            } else {
                return Token(line, column, "=", TokenType::ASSIGNMENT);
            }
        } else if (currChar == '<' || currChar == '>') {
            char nextChar = sExpression.peek();
            if (nextChar == '=') {
                sExpression.get();
                column++;
                return Token(line, column - 1, std::string(1, currChar) + "=", TokenType::OPERATOR);
            } else {
                return Token(line, column, std::string(1, currChar), TokenType::OPERATOR);
            }
        } else if (currChar == '!') {
            char nextChar = sExpression.peek();
            if (nextChar == '=') {
                sExpression.get();
                column++;
                return Token(line, column - 1, "!=", TokenType::OPERATOR);
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
        } else if (currChar == ',') {  // New: Tokenize comma
            return Token(line, column, ",", TokenType::COMMA);
        } else if (currChar == ';') {  // New: Tokenize semicolon
            return Token(line, column, ";", TokenType::SEMICOLON);
        } else if (std::isdigit(currChar)) {
            std::string num;
            num += currChar;
            char nextChar;

            while (sExpression.get(nextChar)) {
                if (std::isdigit(nextChar) || nextChar == '.') {
                    // Check if the next character following a '.' is a digit
                    if (nextChar == '.' && num.find('.') == std::string::npos) {
                        char followingChar = sExpression.peek();  // Peek at the next character
                        if (!std::isdigit(followingChar)) {      // Check if it's not a digit
                            std::cout << "Syntax error on line " << std::to_string(line) << " column " << std::to_string(column + 2) << "." << std::endl;
                            exit(1);
                        }
                    }
                    column++;

                    if (nextChar == '.' && num.find('.') != std::string::npos) {
                        std::cout << "Syntax error on line " << std::to_string(line) << " column " << std::to_string(column) << "." << std::endl;
                        exit(1);
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
            column += identifier.length() - 1;
            if (identifier == "true" || identifier == "false") {
                return Token(line, tempColumn, identifier, TokenType::BOOLEAN);
            } else {
                return Token(line, tempColumn, identifier, TokenType::IDENTIFIER);
            }
        } else {
            std::cout << "Syntax error on line " << line << " column " << column << "." << std::endl;
            exit(1);
        }
    }

    // If you reach the end of the input, create and return the "END" token
    return Token(line, column + 1, "END", TokenType::OPERATOR);
}

std::vector<Token> Lexer::tokenize() {
    Token currToken = nextToken();

    while (currToken.text != "END") {
        myTokens.push_back(currToken);
        currToken = nextToken();
    }
    myTokens.push_back(currToken);

    return myTokens;
}

Token Lexer::tokenizeIfStatement() {
    return Token(line, column, "if", TokenType::IF);
}

Token Lexer::tokenizeWhileStatement() {
    return Token(line, column, "while", TokenType::WHILE);
}

Token Lexer::tokenizePrintStatement() {
    return Token(line, column, "print", TokenType::PRINT);
}
