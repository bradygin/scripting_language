#include "lexer.h"
#include "token.h"
#include <cctype>
#include <stdexcept>

// Constructor: Initializes Lexer object with input stream
Lexer::Lexer(std::istream& input) : sExpression(input) {}

// Function to fetch the next token from the input stream
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
                    // Check if the next character following a '.' is a digit
                    if (nextChar == '.' && num.find('.') == std::string::npos) {
                        char followingChar = sExpression.peek();  // Peek at the next character
                        if (!std::isdigit(followingChar)) {  // Check if it's not a digit
                            throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column + 2));
                        }
                    }
                    column++;

                      if (nextChar == '.' && num.find('.') != std::string::npos) {
                            throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
                    }

                    num += nextChar;

                } else {
                    sExpression.unget();
                    break;
                }
            }

            return Token(line, (column - num.length() + 1), num, TokenType::NUMBER);
        } else {
            throw std::runtime_error("Syntax error on line " + std::to_string(line) + " column " + std::to_string(column) + ".");
        }
    }

    // If you reach the end of the input, create and return the "END" token
    if (currChar == '\n') {
        return Token(line, 1, "END", TokenType::OPERATOR);
    }
    else {
        return Token(line, column + 1, "END", TokenType::OPERATOR);
    }
}

// Function to tokenize the entire input stream and return a vector of tokens
std::vector<Token> Lexer::tokenize() {
    Token currToken = nextToken();

    while (currToken.text != "END") {  // Check if there are more tokens to read
        myTokens.push_back(currToken);   
        currToken = nextToken();
    }

    myTokens.push_back(currToken);

    return myTokens;
}


