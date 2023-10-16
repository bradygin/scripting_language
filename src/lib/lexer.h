#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include "token.h"

struct Lexer {
public:
    //Defines a constructor for the Lexer struct that takes a reference to an std::istream as an argument.
    Lexer(std::istream& input);
    std::vector<Token> myTokens;
    // A reference to an std::istream
    std::istream& sExpression;

    std::vector<Token> tokenize();
    Token nextToken();

    int line = 1;
    int column = 0;
};

#endif
