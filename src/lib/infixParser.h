#ifndef INFIXPARSER_H
#define INFIXPARSER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <stdexcept>
#include "lexer.h"
#include "token.h"

class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual double evaluate(std::map<std::string, double>& symbolTable /* unused */) const = 0;
    virtual std::string toInfix() const = 0;
};


struct BinaryOperation : public ASTNode {
public:
    BinaryOperation(char op, ASTNode* left, ASTNode* right)
    : op(op), left(left), right(right) {}
    ~BinaryOperation();
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override;
    std::string toInfix() const override;
    char op;
    ASTNode* left;
    ASTNode* right;
};


struct Number : public ASTNode {
public:
    Number(double value) : value(value) {}
    double evaluate(std::map<std::string, double>& /* unused */) const override { return value; }
    std::string toInfix() const override;
    double value;
};


class infixParser {
public:
    infixParser(const std::vector<Token>& tokens);
    std::string printInfix(ASTNode* node);
    ASTNode* infixparse();
    infixParser(const std::vector<Token>& tokens, std::map<std::string, double>& symbolTable);
    Token PeekNextToken();

private:
    std::vector<Token> tokens;
    size_t index;
    Token currentToken;
    std::map<std::string, double>& symbolTable;

    void nextToken();
    ASTNode* infixparsePrimary();
    ASTNode* infixparseExpression();
    ASTNode* infixparseTerm();
    ASTNode* infixparseFactor();
};


class Assignment : public ASTNode {
public:
    Assignment(const std::string& varName, ASTNode* expression);
    ~Assignment();
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override;
    std::string toInfix() const override;
    std::string variableName;
    ASTNode* expression;
};


class Variable : public ASTNode {
public:
    Variable(const std::string& varName) : variableName(varName) {}
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override {
    return variableName;
}
    std::string variableName;
};

//EXCEPTION HANDLING
class UnknownIdentifierException : public std::runtime_error{
public:
    UnknownIdentifierException(std::map<std::string, double>& /* unused */, const std::string& variableName)
    : std::runtime_error("Runtime error: unknown identifier " + variableName) {}

    int getErrorCode() const {
    return 3;
    }
};


class DivisionByZeroException : public std::runtime_error {
public:
    DivisionByZeroException() : std::runtime_error("Runtime error: division by zero.") {}
    
    int getErrorCode() const {
    return 3;
    }
};


class InvalidOperatorException : public std::runtime_error {
public:
    InvalidOperatorException() : std::runtime_error("Invalid operator") {}
    int getErrorCode() const {
    return 2;
    }
};


class UnexpectedTokenException : public std::runtime_error {
public:
    UnexpectedTokenException(const std::string& tokenText, int line, int column)
    : std::runtime_error("Unexpected token at line " + std::to_string(line) + " column " + std::to_string(column) + ": " + tokenText) {}
    int getErrorCode() const {
    return 2;
    }
};


#endif