#ifndef INFIXPARSER_H
#define INFIXPARSER_H

#include <vector>
#include <string>
#include <iostream>
#include "lexer.h"
#include "token.h"

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual double evaluate() const = 0;
    virtual std::string toInfix() const = 0;
    
};
//assist with operators
struct BinaryOperation : public ASTNode {
public:
    BinaryOperation(char op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}

    double evaluate() const override;
    std::string toInfix() const override;


    char op;
    ASTNode* left;
    ASTNode* right;
};

struct Number : public ASTNode {
public:
    Number(double value) : value(value) {}

    double evaluate() const override { return value; }
    std::string toInfix() const override;

    double value;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::string printInfix(ASTNode* node);
    ASTNode* parse();

private:
    std::vector<Token> tokens;
    size_t index;
    Token currentToken;

    void nextToken();
    ASTNode* parsePrimary();
    ASTNode* parseExpression();
    ASTNode* parseTerm();
    ASTNode* parseFactor();
};

#endif

