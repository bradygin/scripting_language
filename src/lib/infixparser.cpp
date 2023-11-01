#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include "infixparser.h"

std::map<std::string, double> symbolTable;

Assignment::Assignment(const std::string& varName, ASTNode* expression)
    : variableName(varName), expression(expression) {}

double Assignment::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = expression->evaluate(symbolTable);
    symbolTable[variableName] = result;
    return result;
}

double Variable::evaluate(std::map<std::string, double>& symbolTable) const {
    if (symbolTable.find(variableName) != symbolTable.end()) {
        return symbolTable.at(variableName);
    } else {
        throw UnknownIdentifierException(symbolTable, variableName);
    }
}

std::string Assignment::toInfix() const {
    return "(" + variableName + " = " + expression->toInfix() + ")";
}

double BinaryOperation::evaluate(std::map<std::string, double>& symbolTable) const {
    double leftValue = left->evaluate(symbolTable);
    double rightValue = right->evaluate(symbolTable);

    switch (op) {
        case '+': return leftValue + rightValue;
        case '-': return leftValue - rightValue;
        case '*': return leftValue * rightValue;
        case '/':
            if (rightValue == 0) {
                throw DivisionByZeroException();
            }
            return leftValue / rightValue;
        case '%':
            if (rightValue == 0) {
                throw DivisionByZeroException();
            }
            return std::fmod(leftValue, rightValue);
        case '<': return leftValue < rightValue;
        case '<=': return leftValue <= rightValue;
        case '>': return leftValue > rightValue;
        case '>=': return leftValue >= rightValue;
        case '==':
            if (isBool(leftValue) && isBool(rightValue)) {
                return leftValue == rightValue;
            } else {
                throw InvalidOperandTypeException();
            }
        case '!=':
            if (isBool(leftValue) && isBool(rightValue)) {
                return leftValue != rightValue;
            } else {
                throw InvalidOperandTypeException();
            }
        case '&':
            if (isBool(leftValue) && isBool(rightValue)) {
                return leftValue && rightValue;
            } else {
                throw InvalidOperandTypeException();
            }
        case '^':
            if (isBool(leftValue) && isBool(rightValue)) {
                return leftValue != rightValue;
            } else {
                throw InvalidOperandTypeException();
            }
        case '|':
            if (isBool(leftValue) && isBool(rightValue)) {
                return leftValue || rightValue;
            } else {
                throw InvalidOperandTypeException();
            }
        default:
            throw InvalidOperatorException();
    }
}

bool BinaryOperation::isBool(double value) const {
    return value == 0.0 || value == 1.0;
}

std::string BinaryOperation::toInfix() const {
    std::string leftStr = left->toInfix();
    std::stringstream ss;
    if (op == '==') {
        ss << leftStr << " == " << right->toInfix();
    } else if (op == '!=') {
        ss << leftStr << " != " << right->toInfix();
    } else {
        ss << "(" << leftStr << " " << op << " " << right->toInfix() << ")";
    }
    return ss.str();
}

std::string Number::toInfix() const {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

infixParser::infixParser(const std::vector<Token>& tokens, std::map<std::string, double>& symbolTable)
    : tokens(tokens), index(0), symbolTable(symbolTable) {
    if (!tokens.empty()) {
        currentToken = tokens[index];
    }
}

void infixParser::nextToken() {
    if (index < tokens.size() - 1) {
        index++;
        currentToken = tokens[index];
    } else {
        // when END is reached
        currentToken = Token(0, 0, "END", TokenType::OPERATOR);
    }
}

ASTNode* infixParser::infixparse() {
    return infixparseExpression();
}

ASTNode* infixParser::infixparseExpression() {
    std::unique_ptr<ASTNode> left(infixparseTerm());

    while (currentToken.type == TokenType::OPERATOR &&
           (currentToken.text == "+" || currentToken.text == "-")) {
        char op = currentToken.text[0];
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseTerm());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

BinaryOperation::~BinaryOperation() {
    delete left;
    delete right;
}

Assignment::~Assignment() {
    delete expression;
}

ASTNode* infixParser::infixparseTerm() {
    std::unique_ptr<ASTNode> left(infixparseFactor());

    while (currentToken.type == TokenType::OPERATOR &&
           (currentToken.text == "*" || currentToken.text == "/" || currentToken.text == "%")) {
        char op = currentToken.text[0];
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseFactor());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseFactor() {
    return infixparsePrimary();
}

ASTNode* infixParser::infixparsePrimary() {
    if (currentToken.type == TokenType::NUMBER) {
        double value = std::stod(currentToken.text);
        nextToken();
        return std::make_unique<Number>(value).release();
    } else if (currentToken.type == TokenType::BOOLEAN) {
        bool value = currentToken.text == "true";
        nextToken();
        return std::make_unique<Number>(value ? 1.0 : 0.0).release();
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.text;
        nextToken();
        if (currentToken.type == TokenType::ASSIGNMENT) {
            nextToken();
            std::unique_ptr<ASTNode> expr(infixparseExpression());
            return std::make_unique<Assignment>(varName, expr.release()).release();
        } else {
            return std::make_unique<Variable>(varName).release();
        }
    } else if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken();
        std::unique_ptr<ASTNode> result(infixparseExpression());
        if (currentToken.type == TokenType::RIGHT_PAREN) {
            nextToken();
            return result.release();
        } else {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
    } else if (currentToken.type == TokenType::RIGHT_PAREN) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    } else {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
}

Token infixParser::PeekNextToken() {
    if (index < tokens.size() - 1) {
        return tokens[index + 1];
    }
    return Token(0, 0, "END", TokenType::OPERATOR);
}

std::string infixParser::printInfix(ASTNode* node) {
    if (dynamic_cast<BinaryOperation*>(node) != nullptr) {
        BinaryOperation* binOp = dynamic_cast<BinaryOperation*>(node);
        std::string leftStr = printInfix(binOp->left);
        std::string rightStr = printInfix(binOp->right);
        return "(" + leftStr + " " + binOp->op + " " + rightStr + ")";
    } else if (dynamic_cast<Number*>(node) != nullptr) {
        std::ostringstream oss;
        oss << dynamic_cast<Number*>(node)->value;
        return oss.str();
    } else if (dynamic_cast<Assignment*>(node) != nullptr) {
        Assignment* assignment = dynamic_cast<Assignment*>(node);
        return "(" + assignment->variableName + " = " + printInfix(assignment->expression) + ")";
    } else if (dynamic_cast<Variable*>(node) != nullptr) {
        Variable* variable = dynamic_cast<Variable*>(node);
        return variable->variableName;
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}
