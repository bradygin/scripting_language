#ifndef INFIXLEXER_H
#define INFIXLEXER_H

#include <iostream>
#include <vector>
#include <stdexcept>
#include "token.h"


//Lexer class definition
struct Lexer {
public:
    //Defines a constructor for the Lexer struct that takes a reference to an std::istream as an argument.
    Lexer(std::istream& input);
    std::vector<Token> myTokens;
    // A reference to an std::istream
    std::istream& sExpression;

    std::vector<Token> infixtokenize();
    Token infixnextToken();

    int line = 1;
    int column = 0;
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