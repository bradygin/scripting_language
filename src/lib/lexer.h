
#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <vector>
#include "token.h"

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

#endif