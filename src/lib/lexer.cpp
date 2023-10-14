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
            char nextChar;
            while (sExpression.get(nextChar)) {
                if (std::isdigit(nextChar) || nextChar == '.') {
                    num += nextChar;
                } else {
                    sExpression.unget();
                    break;
                }
                // Increment column only when a digit is added to the number
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

    // SYNTAX ERROR, CAN'T CREATE TOKEN
        } else {
            throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
        }
    }

    // NO LONGER A NEXT CHARACTER SO CREATE SPECIAL END TOKEN
    return Token(line, column, "END", TokenType::OPERATOR);
}


std::vector<Token> Lexer::tokenize() {
    // GET THE FIRST TOKEN
    Token currToken = nextToken();   

    //std::cout << currToken.line << " " << currToken.column << " " << currToken.text << std::endl;

    // GET ALL THE TOKENS IN THE STREAM AND ADD THEM TO THE VECTOR
    while (currToken.text != "END") {
        std::cout << currToken.line << " " << currToken.column << " " << currToken.text << std::endl;
        myTokens.push_back(currToken);
        currToken = nextToken();

        if (currToken.text == "END"){
            break;
        }
    }

    // Push the "END" token after the loop.
    myTokens.push_back(currToken);

    std::cout << currToken.line << " " << currToken.column << " " << currToken.text << std::endl;

    return myTokens;
}

