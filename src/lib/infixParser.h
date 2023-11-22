#ifndef INFIXPARSER_H
#define INFIXPARSER_H

#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>
#include <stdexcept>
#include "lexer.h"
#include "token.h"

static std::map<std::string, double> symbolTable;

// FOR ARRAYS: Define Expr and ExprPtr
class Expr;
using ExprPtr = std::shared_ptr<Expr>;

// Define Expr and ExprPtr
class Expr {
public:
    virtual ~Expr() {}
};


// Class for node
class ASTNode {
  public:
    virtual ~ASTNode() {}
    virtual double evaluate(std::map<std::string, double>& symbolTable) = 0;
    virtual std::string toInfix() const = 0;
};

struct BinaryOperation : public ASTNode {
public:
    BinaryOperation(const std::string& op, std::shared_ptr<ASTNode> left, std::shared_ptr<ASTNode> right)
    : op(op), left(left), right(right) {}
    ~BinaryOperation() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override;
    std::string toInfix() const override;
    std::string op; 
    std::shared_ptr<ASTNode> left{nullptr};
    std::shared_ptr<ASTNode> right{nullptr};
};

class BooleanNode : public ASTNode {
  public:
    BooleanNode(bool value);
    double evaluate(std::map<std::string, double>& symbolTable) override;
    std::string toInfix() const override;

  private:
    bool value;
};


struct Number : public ASTNode {
  public:
    Number(double value) : value(value) {}
    double evaluate(std::map<std::string, double>&) override { return value; }
    std::string toInfix() const override;
    double value;
};

class Assignment : public ASTNode {
  public:
    Assignment(const std::string& varName, std::shared_ptr<ASTNode> expression);
    ~Assignment() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override;
    std::string toInfix() const override;
    std::string variableName;
    std::shared_ptr<ASTNode> expression{nullptr};
};


class Variable : public ASTNode {
  public:
    Variable(const std::string& varName) : variableName(varName) {}
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override {
        return variableName;
    }
    std::string variableName;
};


class Block : public ASTNode {
  public:
    Block(std::shared_ptr<ASTNode> statement);
    ~Block() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::vector<std::shared_ptr<ASTNode>> statements;
};

class BracedBlock : public ASTNode {
  public:
    BracedBlock(std::shared_ptr<Block> blk);
    ~BracedBlock() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::shared_ptr<Block> block{nullptr};
};

class ElseStatement;
class IfStatement : public ASTNode {
  public:
    IfStatement(std::shared_ptr<ASTNode> cond, std::shared_ptr<BracedBlock> blk);
    ~IfStatement() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::shared_ptr<ASTNode> condition{nullptr};
    std::shared_ptr<BracedBlock> bracedBlock{nullptr};
    std::shared_ptr<ElseStatement> elseNode{nullptr};
};

class ElseStatement : public ASTNode {
  public:
    ElseStatement(std::shared_ptr<IfStatement> state, std::shared_ptr<BracedBlock> blk);
    ~ElseStatement() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override;
    std::string toInfix() const override;
    std::shared_ptr<IfStatement> ifStatement{nullptr};
    std::shared_ptr<BracedBlock> bracedBlock{nullptr};
};

class WhileStatement : public ASTNode {
  public:
    WhileStatement(std::shared_ptr<ASTNode> cond, std::shared_ptr<BracedBlock> blk);
    ~WhileStatement() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::shared_ptr<ASTNode> condition{nullptr};
    std::shared_ptr<BracedBlock> bracedBlock{nullptr};
};

class PrintStatement : public ASTNode {
  public:
    PrintStatement(std::shared_ptr<ASTNode> expression);
    ~PrintStatement() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::shared_ptr<ASTNode> expression{nullptr};
};

class FunctionReturn;
class FunctionCall;
class FunctionDefinition : public ASTNode {
  public:
    FunctionDefinition(std::string name);
    ~FunctionDefinition() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    bool isCalled{false};
    std::string functionName;
    std::string functionAliasName;
    std::map<std::string, double> mySymbolTable;
    std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> parameters;
    std::shared_ptr<BracedBlock> bracedBlock{nullptr};
};

class FunctionReturn : public ASTNode {
  public:
    FunctionReturn(std::shared_ptr<ASTNode> expression);
    ~FunctionReturn() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::shared_ptr<ASTNode> expression{nullptr};
};

class FunctionCall : public ASTNode {
  public:
    FunctionCall(std::string name);
    ~FunctionCall() = default;
    double evaluate(std::map<std::string, double>& symbolTable) override; 
    std::string toInfix() const override;
    std::string functionName;
    bool isAliasName{false};
    std::vector<std::pair<std::string, std::shared_ptr<ASTNode>>> parameters;
};


// ADDED FOR ARRAYS
// Forward declaration for new expression types
class ArrayLiteralExpr;
class ArrayLookupExpr;
class ArrayAssignExpr;


// Define ArrayLiteral
class ArrayLiteral : public ASTNode {
public:
    ArrayLiteral() = default;  // Add this line for the default constructor

    ArrayLiteral(const std::vector<std::shared_ptr<ASTNode>>& elements)
        : elements(elements) {}

    double evaluate(std::map<std::string, double>& symbolTable) override;
    std::string toInfix() const override;
    std::vector<std::shared_ptr<ASTNode>> elements;
};


class ArrayLookup : public ASTNode {
public:
    ArrayLookup(std::shared_ptr<ASTNode> array, std::shared_ptr<ASTNode> index)
        : array(array), index(index) {}

    double evaluate(std::map<std::string, double>& symbolTable) override;
    std::string toInfix() const override;
    std::shared_ptr<ASTNode> array;
    std::shared_ptr<ASTNode> index;

    void setAssignmentValue(std::shared_ptr<ASTNode> value) {
        // Check if the array and index are valid
        if (!array || !index) {
            throw std::runtime_error("Array and index cannot be null.");
        }

        // Evaluate the array and index to get their values
        //double arrayValue = array->evaluate(symbolTable);
        double indexValue = index->evaluate(symbolTable);

        // Check if the array is a valid array (ArrayLiteral)
        auto arrayLiteral = std::dynamic_pointer_cast<ArrayLiteral>(array);
        if (!arrayLiteral) {
            throw std::runtime_error("Invalid array type for assignment.");
        }

        // Check if the index is a valid integer
        if (std::floor(indexValue) != indexValue) {
            throw std::runtime_error("Array index must be an integer.");
        }

        int arrayIndex = static_cast<int>(indexValue);

        // Check if the array index is within bounds
        if (arrayIndex < 0 || arrayIndex >= static_cast<int>(arrayLiteral->elements.size())) {
            throw std::runtime_error("Array index out of bounds.");
        }

        // Update the array element with the new value
        arrayLiteral->elements[arrayIndex] = value;
    }
};

class ArrayAssignExpr : public Expr {
public:
    ExprPtr array;
    ExprPtr index;
    ExprPtr value;

    ArrayAssignExpr(ExprPtr array, ExprPtr index, ExprPtr value)
        : array(array), index(index), value(value) {}
};

// END FOR ARRAYS



class infixParser {
public:
    infixParser(const std::vector<Token>& tokens);
    std::string printInfix(std::shared_ptr<ASTNode> node);
    std::shared_ptr<ASTNode> infixparse();
    Token PeekNextToken();
    double evaluate(std::shared_ptr<ASTNode> node);

    // Add array utility functions
    double getArrayLength(std::shared_ptr<ASTNode> array);
    double arrayPop(std::shared_ptr<ASTNode> array);
    void arrayPush(std::shared_ptr<ASTNode> array, double value);

    // Add array parsing functions
    std::shared_ptr<ArrayLiteral> infixparseArrayLiteral();
    std::shared_ptr<ArrayLookup> infixparseArrayLookup(std::shared_ptr<ASTNode> array);
    std::shared_ptr<ASTNode> infixparseArrayAssignment(std::shared_ptr<ASTNode> array);

    // Update infixparseExpression and infixparsePrimary
    std::shared_ptr<ASTNode> infixparsePrimary();

private:
    std::vector<Token> tokens;
    size_t index;
    Token currentToken;

    void nextToken();
    std::shared_ptr<ASTNode> infixparseExpression();
    std::shared_ptr<ASTNode> infixparseTerm();
    std::shared_ptr<ASTNode> infixparseFactor();
    std::shared_ptr<ASTNode> infixparseAssignment();
    std::shared_ptr<ASTNode> infixparseComparison();
    std::shared_ptr<ASTNode> infixparseEquality();
    std::shared_ptr<ASTNode> infixparseLogicalAnd();
    std::shared_ptr<ASTNode> infixparseLogicalOr();
    std::shared_ptr<ASTNode> infixparseLogicalXor();
    std::shared_ptr<ASTNode> infixparseStatement();
    std::shared_ptr<ASTNode> infixparseCondition();
    std::shared_ptr<BracedBlock> infixparseBracedBlock();
    std::shared_ptr<IfStatement> infixparseIfStatement();
    std::shared_ptr<ElseStatement> infixparseElseStatement();
    std::shared_ptr<FunctionDefinition> infixparseFunctionDefinition();
    std::shared_ptr<FunctionCall> infixparseFunctionCall();
};



//EXCEPTION HANDLING
class UnknownIdentifierException : public std::runtime_error{
  public:
    UnknownIdentifierException(std::map<std::string, double>&, const std::string& variableName)
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
    : std::runtime_error("0  Unexpected token at line " + std::to_string(line) + " column " + std::to_string(column) + ": " + tokenText) {}
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
