
#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include "token.h"
#include <stdexcept>

// Lexer class definition
struct Lexer {
public:
    // Constructor declaration: takes an input stream
    Lexer(std::istream& input);
    
    // Member variable to hold tokens
    std::vector<Token> myTokens;
    
    // Member variable to hold the input stream
    std::istream& sExpression;
    
    // Member variables to track line and column numbers
    int line = 1;
    int column = 0;
    
    // Function declaration for tokenization
    std::vector<Token> tokenize();
    
    // Function declaration for fetching the next token
    Token nextToken();
};

class SyntaxError : public std::runtime_error {
public:
    SyntaxError(int line, int column)
    : std::runtime_error ("Syntax error on line " + std::to_string(line) + " column " +std::to_string(column) + ".") {}
    int getErrorCode() const {
    return 1;
    }
};

#endif