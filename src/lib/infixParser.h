#ifndef INFIXPARSER_H
#define INFIXPARSER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "lexer.h"
#include "token.h"

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual double evaluate() const = 0;
    virtual std::string toInfix() const = 0;
    
};

struct BinaryOperation : public ASTNode {
public:
    BinaryOperation(char op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}
    ~BinaryOperation();
    double evaluate() const override;
    std::string toInfix() const override;


    char op;
    ASTNode* left;
    ASTNode* right;
};

struct Number : public ASTNode {
public:
    Number(double value) : value(value) {}
    ~Number();
    double evaluate() const override { return value; }
    std::string toInfix() const override;

    double value;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::string printInfix(ASTNode* node);
    ASTNode* parse();
    ~Parser();

private:
    std::vector<Token> tokens;
    size_t index;
    Token currentToken;
    std::map<std::string, double> variables;

    void nextToken();
    ASTNode* parsePrimary();
    ASTNode* parseExpression();
    ASTNode* parseTerm();
    ASTNode* parseFactor();
};

class Assignment : public ASTNode {
public:
    Assignment(const std::string& varName, ASTNode* expression);
    ~Assignment();
    double evaluate() const override;
    std::string toInfix() const override;

    std::string variableName;
    ASTNode* expression;
};

class Variable : public ASTNode {
public:
    Variable(const std::string& varName) : variableName(varName) {}

    double evaluate() const override {
        return 0.0; 
    }

    std::string toInfix() const override {
        return variableName;
    }


    std::string variableName;
};

#endif

