#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <iostream>
#include "lexer.h"
#include "token.h"
//for constructing AST
class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual double evaluate() const = 0;
    virtual std::string toInfix() const = 0;
};
//assist with operators
class BinaryOperation : public ASTNode {
public:
    BinaryOperation(char op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}

    double evaluate() const override;
    std::string toInfix() const override;


    char op;
    ASTNode* left;
    ASTNode* right;
};

class Number : public ASTNode {
public:
    Number(double value) : value(value) {}

    double evaluate() const override { return value; }
    std::string toInfix() const override;

private:
    double value;
};
//actually parse the tokens
class Parser {
public:
    Parser(Lexer& lexer) : lexer(lexer) {}
    std::string print(ASTNode* node, int indent); //print tree
    ASTNode* parse();

private:
    Lexer& lexer;
    Token currentToken;

    void nextToken();
    ASTNode* parsePrimary();
    ASTNode* parseExpression();
    ASTNode* parseTerm();
    ASTNode* parseFactor();
};

#endif
