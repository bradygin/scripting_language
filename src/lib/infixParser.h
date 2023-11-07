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


struct Number : public ASTNode {
public:
    Number(double value) : value(value) {}
    double evaluate(std::map<std::string, double>& /* unused */) const override { return value; }
    std::string toInfix() const override;
    double value;
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

class BooleanNode : public ASTNode {
  public:
    BooleanNode(bool value) : value(value) {}
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override {
        return value ? "true" : "false";
    }
    bool value;
};

class Block : public ASTNode {
  public:
    Block(ASTNode* statement);
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override;
    std::vector<ASTNode*> statements;
};

class BracedBlock : public ASTNode {
  public:
    BracedBlock(Block* blk);
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override;
    Block* block;
};

class ElseStatement;
class IfStatement : public ASTNode {
  public:
    IfStatement(ASTNode* cond, BracedBlock* blk);
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override;
    ASTNode* condition;
    BracedBlock* bracedBlock;
    ElseStatement* elseNode;
};

class ElseStatement : public ASTNode {
  public:
    ElseStatement(IfStatement* state, BracedBlock* blk);
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override;
    std::string toInfix() const override;
    IfStatement* ifStatement;
    BracedBlock* bracedBlock;
};

class WhileStatement : public ASTNode {
  public:
    WhileStatement(ASTNode* cond, BracedBlock* blk);
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override;
    ASTNode* condition;
    BracedBlock* bracedBlock;

};

class PrintStatement : public ASTNode {
  public:
    PrintStatement(ASTNode* expression);
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override; 
    std::string toInfix() const override;
    ASTNode* expression;
};

class EndStatement : public ASTNode {
  public:
    EndStatement() = default;
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override;
    std::string toInfix() const override { return "}"; }
};

class EmptyStatement : public ASTNode {
  public:
    EmptyStatement() = default;
    double evaluate(std::map<std::string, double>& symbolTable /* unused */) const override;
    std::string toInfix() const override { return {}; }
};

class infixParser {
public:
    infixParser(const std::vector<Token>& tokens);
    std::string printInfix(ASTNode* node);
    double evaluate(ASTNode* node, std::map<std::string, double>& symbolTable);
    std::vector<ASTNode*> infixparse();
    infixParser(const std::vector<Token>& tokens, std::map<std::string, double>& symbolTable);
    Token PeekNextToken();

private:
    std::vector<Token> tokens;
    size_t index;
    Token currentToken;
    std::map<std::string, double>& symbolTable;
    std::vector<ASTNode*> roots;

    void nextToken();
    ASTNode* infixparsePrimary();
    ASTNode* infixparseExpression();
    ASTNode* infixparseTerm();
    ASTNode* infixparseFactor();
    ASTNode* infixparseStatement();
    
    ASTNode* infixparseCondition();
    BracedBlock* infixparseBracedBlock();
    IfStatement* infixparseIfStatement();
    ElseStatement* infixparseElseStatement();
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
    : std::runtime_error("8 Unexpected token at line " + std::to_string(line) + " column " + std::to_string(column) + ": " + tokenText) {}
    int getErrorCode() const {
    return 2;
    }
};


#endif