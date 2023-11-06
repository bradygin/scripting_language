#ifndef INFIXPARSER_H
#define INFIXPARSER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <stdexcept>
#include "lexer.h"
#include "token.h"

// Class for node
class ASTNode {
public:
    virtual ~ASTNode() {}
    virtual double evaluate(std::map<std::string, double>& symbolTable /* unused */) const = 0;
    virtual std::string toInfix() const = 0;
};

class Statement : public ASTNode {
public:
    virtual ~Statement() {}
    virtual double evaluate(std::map<std::string, double>& symbolTable) const override = 0;
};

class Block : public Statement {
public:
    Block(const std::vector<Statement*>& statements);
    ~Block();
    double evaluate(std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;

private:
    std::vector<Statement*> statements;
};

class IfStatement : public Statement {
public:
    IfStatement(ASTNode* condition, Statement* ifBlock, Statement* elseBlock = nullptr);
    ~IfStatement();
    double evaluate(std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;

private:
    ASTNode* condition;
    Statement* ifBlock;
    Statement* elseBlock;
};

class WhileStatement : public Statement {
public:
    WhileStatement(ASTNode* condition, Statement* loopBlock);
    ~WhileStatement();
    double evaluate(std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;

private:
    ASTNode* condition;
    Statement* loopBlock;
};

class PrintStatement : public Statement {
public:
    PrintStatement(ASTNode* expression);
    ~PrintStatement();
    double evaluate(std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;

private:
    ASTNode* expression;
};


struct BinaryOperation : public ASTNode {
public:
    BinaryOperation(const std::string& op, ASTNode* left, ASTNode* right)
    : op(op), left(left), right(right) {}
    ~BinaryOperation();
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override;
    std::string toInfix() const override;
    std::string op; 
    ASTNode* left;
    ASTNode* right;
};

class BooleanNode : public ASTNode {
public:
    BooleanNode(bool value);
    double evaluate(std::map<std::string, double>& symbolTable) const override;
    std::string toInfix() const override;

private:
    bool value;
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
    Token currentToken;

private:
    std::vector<Token> tokens;
    size_t index;
    std::map<std::string, double>& symbolTable;

    void nextToken();
    ASTNode* infixparsePrimary();
    ASTNode* infixparseExpression();
    ASTNode* infixparseTerm();
    ASTNode* infixparseFactor();
    ASTNode* infixparseAssignment();
    ASTNode* infixparseComparison();
    ASTNode* infixparseEquality();
    ASTNode* infixparseLogicalAnd();
    ASTNode* infixparseLogicalOr();
    ASTNode* infixparseLogicalXor();
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

class InvalidOperandTypeException : public std::runtime_error {
public:
    InvalidOperandTypeException() : std::runtime_error("Runtime error: invalid operand type.") {}

    int getErrorCode() const {
        return 3; 
    }
};

#endif