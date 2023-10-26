#ifndef INFIXPARSER_H
#define INFIXPARSER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "lib/lexer.h"
#include "lib/token.h"

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual double evaluate(const std::map<std::string, double>& symbolTable) const = 0;
    virtual std::string toInfix() const = 0;
    
};

struct BinaryOperation : public ASTNode {
public:
    BinaryOperation(char op, ASTNode* left, ASTNode* right)
        : op(op), left(left), right(right) {}

    ~BinaryOperation();

    double evaluate(const std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;


    char op;
    ASTNode* left;
    ASTNode* right;
};

struct Number : public ASTNode {
public:
    Number(double value) : value(value) {}

    double evaluate(const std::map<std::string, double>& symbolTable) const override { return value; }
    std::string toInfix() const override;

    double value;
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::string printInfix(ASTNode* node);
    ASTNode* parse();
    Parser(const std::vector<Token>& tokens, std::map<std::string, double>& symbolTable);

private:
    std::vector<Token> tokens;
    size_t index;
    Token currentToken;
    std::map<std::string, double>& symbolTable;

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

    double evaluate(const std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;

    std::string variableName;
    ASTNode* expression;
};

class Variable : public ASTNode {
public:
    Variable(const std::string& varName) : variableName(varName) {}

    double evaluate(const std::map<std::string, double>& symbolTable) const  {
        if (symbolTable.find(variableName) != symbolTable.end()) {
            return symbolTable.at(variableName);
        } else {
            std::cout << "Runtime error: unknown identifier " << variableName << std::endl;
            exit(3);
        }
    }

    std::string toInfix() const override {
        return variableName;
    }

    std::string variableName;
};



#endif