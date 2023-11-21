#ifndef ARRAY_H
#define ARRAY_H

#include "infixParser.h"
#include "lexer.h"
#include "token.h"
#include <variant>

class ArrayVariable : public ASTNode {
public:
    ArrayVariable(const std::string& varName, std::unique_ptr<ASTNode> index);
    double evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const;
    std::string toInfix() const override;

private:
    std::string arrayName;
    std::unique_ptr<ASTNode> index;
};

class ArrayAssignment : public ASTNode {
public:
    ArrayAssignment(const std::string& varName, std::unique_ptr<ASTNode> index, std::unique_ptr<ASTNode> expression);
    double evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const;
    std::string toInfix() const override;

private:
    std::string arrayName;
    std::unique_ptr<ASTNode> index;
    std::unique_ptr<ASTNode> expression;
};

class ArrayIndex : public ASTNode {
public:
    ArrayIndex(std::unique_ptr<ASTNode> array, std::unique_ptr<ASTNode> index);
    double evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const;
    std::string toInfix() const override;

private:
    std::unique_ptr<ASTNode> array;
    std::unique_ptr<ASTNode> index;
};

class ArrayLiteral : public ASTNode {
public:
    ArrayLiteral(const std::vector<std::shared_ptr<ASTNode>>& elements);
    std::variant<double, std::vector<double>> evaluate(std::map<std::string, std::variant<double, std::vector<double>>>& symbolTable) const;
    std::string toInfix() const override;

private:
    std::vector<std::shared_ptr<ASTNode>> elements;
};

//UTILITY FUNCTIONS
double len(const std::vector<double>& array);
double pop(std::vector<double>& array);
double push(std::vector<double>& array, double value);

//ERROR HANDLING
class NumberIndexError : public std::runtime_error {
public:
    NumberIndexError() : std::runtime_error("Runtime error: index is not a number.") {}

    int getErrorCode() const {
        return 3; 
    }
};

class IntegerIndexError : public std::runtime_error {
public:
    IntegerIndexError() : std::runtime_error("Runtime error: index is not an integer.") {}

    int getErrorCode() const {
        return 3; 
    }
};

class NotAnArrayError : public std::runtime_error {
public:
    NotAnArrayError() : std::runtime_error("Runtime error: not an array.") {}

    int getErrorCode() const {
        return 3; 
    }
};

class OutOfBoundsError : public std::runtime_error {
public:
    OutOfBoundsError() : std::runtime_error("Runtime error: index out of bounds.") {}

    int getErrorCode() const {
        return 3; 
    }
};

class UnderError : public std::runtime_error {
public:
    UnderError() : std::runtime_error("Runtime error: underflow.") {}

    int getErrorCode() const {
        return 3; 
    }
};

#endif
