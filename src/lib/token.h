#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    LEFT_PAREN,
    RIGHT_PAREN,
    OPERATOR,
    NUMBER,  
    END
};

struct Token {
    int line;
    int column;      
    std::string text;
    TokenType type;
    Token() : line(0), column(0), text(""), type(TokenType::OPERATOR) {} //default constructor

    Token(int line, int column, const std::string& text, TokenType type)
        : line(line), column(column), text(text), type(type) {}
};

#endif 
