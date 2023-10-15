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
    
    while (sExpression >> currChar) {
        if (currChar == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        std::cout << "DEBUG #1" << " " << currChar << " " << column << std::endl;

        if (std::isspace(currChar)) {
            // Skip whitespace
            continue;
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
                    column++;
                    std::cout << "DEBUG #2" << " " << nextChar << " " << column << std::endl;  

                      if (nextChar == '.' && num.find('.') != std::string::npos) {
                        if (num.front() == '.' || num.back() == '.' || num.find('.') != num.rfind('.')) {
                            throw std::runtime_error(" #1: Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
                        }
                    }

                } else {
                    sExpression.unget();
                    // should throw syntax error here
                }
            }

            return Token(line, column, num, TokenType::NUMBER);

        } else {
            throw std::runtime_error("#2: Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
        }
    }

    // If you reach the end of the input, create and return the "END" token
    return Token(line, column, "END", TokenType::OPERATOR);
}



std::vector<Token> Lexer::tokenize() {
    Token currToken = nextToken();

    while (currToken.text != "END") {  // Check if there are more tokens to read
        myTokens.push_back(currToken);

        std::cout << currToken.line << " " << currToken.column << " " << currToken.text << std::endl;
        
        currToken = nextToken();
    }

    myTokens.push_back(currToken);
    
    std::cout << "FINAL:" << currToken.line << " " << currToken.column << " " << currToken.text << std::endl;

    return myTokens;
}


