
#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include "token.h"
#include <stdexcept>


struct Lexer {
public:

    Lexer(std::istream& input);

    std::vector<Token> myTokens;
    
    std::istream& sExpression;

    int line = 1;
    int column = 0;
    
    std::vector<Token> tokenize();

    Token nextToken();
    Token tokenizeIfStatement();
    Token tokenizeWhileStatement();
    Token tokenizePrintStatement();
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