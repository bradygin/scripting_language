#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include "infixParser.h"


std::map<std::string, double> symbolTable;

Assignment::Assignment(const std::string& varName, ASTNode* expr)  // Change the parameter name
    : variableName(varName), expression(expr) {}  // Update the member name


void infixParser::nextToken() {
    if (index < tokens.size() - 1) {
        index++;
        currentToken = tokens[index];
    } else {
        // when END is reached
        currentToken = Token(0, 0, "END", TokenType::OPERATOR);
    }
}

double Block::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    for (const Statement* statement : statements) {
        result = statement->evaluate(symbolTable);
    }
    return result;
}

double IfStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    double conditionValue = condition->evaluate(symbolTable);
    if (conditionValue != 0.0) {
        result = ifBlock->evaluate(symbolTable);
    } else if (elseBlock) {
        result = elseBlock->evaluate(symbolTable);
    }
    return result;
}

double WhileStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    double conditionValue = condition->evaluate(symbolTable);
    while (conditionValue != 0.0) {
        result = loopBlock->evaluate(symbolTable);
        conditionValue = condition->evaluate(symbolTable);
    }
    return result;
}

double PrintStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = expression->evaluate(symbolTable);
    std::cout << result << std::endl;
    return result;
}

double Assignment::evaluate(std::map<std::string, double>& symbolTable) const {
    if (expression) {
        double result = expression->evaluate(symbolTable);
        symbolTable[variableName] = result;
        return result;
    } else {
        throw std::runtime_error("Assignment has no expression");
    }
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

    // Type checking for arithmetic operations
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        if (dynamic_cast<BooleanNode*>(left) || dynamic_cast<BooleanNode*>(right)) {
            throw InvalidOperandTypeException();
        }
    }
    // Type checking for comparison operations
    if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
        if ((dynamic_cast<BooleanNode*>(left) && !dynamic_cast<BooleanNode*>(right)) ||
            (!dynamic_cast<BooleanNode*>(right))) {
            throw InvalidOperandTypeException();
        }
    }
    // Type checking for logical operations
    if (op == "&" || op == "^" || op == "|") {
        if ((leftValue != 1.0 && leftValue != 0.0) || (rightValue != 1.0 && rightValue != 0.0)) {
            throw InvalidOperandTypeException();
        }
    }

    if (op == "+") return leftValue + rightValue;
    if (op == "-") return leftValue - rightValue;
    if (op == "*") return leftValue * rightValue;
    if (op == "/") {
        if (rightValue == 0) {
            throw DivisionByZeroException();
        }
        return leftValue / rightValue;
    }
    if (op == "%") {
        return std::fmod(leftValue, rightValue);
    }
    if (op == "<") return leftValue < rightValue ? 1 : 0;
    if (op == ">") return leftValue > rightValue ? 1 : 0;
    if (op == "<=") return leftValue <= rightValue ? 1 : 0;
    if (op == ">=") return leftValue >= rightValue ? 1 : 0;
    if (op == "==") return leftValue == rightValue ? 1 : 0;
    if (op == "!=") return leftValue != rightValue ? 1 : 0;
    if (op == "&") return static_cast<int>(leftValue) & static_cast<int>(rightValue);
    if (op == "^") return static_cast<int>(leftValue) ^ static_cast<int>(rightValue);
    if (op == "|") return static_cast<int>(leftValue) | static_cast<int>(rightValue);

    throw InvalidOperatorException();
}

std::string BinaryOperation::toInfix() const {
    std::string leftStr = left->toInfix();
    std::string rightStr = right->toInfix();
    return "(" + leftStr + " " + op + " " + rightStr + ")";
}

std::string Number::toInfix() const {
    std::ostringstream oss;
    oss << value;
    std::string num = oss.str();
    return num;
}

BooleanNode::BooleanNode(bool value) : value(value) {}

double BooleanNode::evaluate(std::map<std::string, double>& /*unused*/) const {
    return value ? 1.0 : 0.0;
}

std::string BooleanNode::toInfix() const {
    return value ? "true" : "false";
}

infixParser::infixParser(const std::vector<Token>& tokens, std::map<std::string, double>& symbolTable)
    : tokens(tokens), index(0), symbolTable(symbolTable) {
    if (!tokens.empty()) {
        currentToken = tokens[index];
    }
}

ASTNode* infixParser::infixparse() {
    return infixparseAssignment();
}

ASTNode* infixParser::infixparseExpression() {
    return infixparseAssignment();
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
        (currentToken.text == "+" || currentToken.text == "-")) {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseFactor());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseComparison() {
    std::unique_ptr<ASTNode> left(infixparseTerm());

    while (currentToken.type == TokenType::OPERATOR &&
        (currentToken.text == "<" || currentToken.text == ">" ||
            currentToken.text == "<=" || currentToken.text == ">=")) {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseTerm());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseLogicalAnd() {
    std::unique_ptr<ASTNode> left(infixparseEquality());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "&") {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseEquality());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseLogicalXor() {
    std::unique_ptr<ASTNode> left(infixparseLogicalAnd());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "^") {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseLogicalAnd());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseLogicalOr() {
    std::unique_ptr<ASTNode> left(infixparseLogicalXor());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "|") {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseLogicalXor());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseAssignment() {
    std::unique_ptr<ASTNode> left(infixparseLogicalOr());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "=") {
        std::string varName = dynamic_cast<Variable*>(left.get())->variableName;
        nextToken();
        std::unique_ptr<ASTNode> expr(infixparseLogicalOr());
        left = std::make_unique<Assignment>(varName, expr.release());
    }

    return left.release();
}


ASTNode* infixParser::infixparseEquality() {
    std::unique_ptr<ASTNode> left(infixparseComparison());

    while (currentToken.type == TokenType::OPERATOR &&
        (currentToken.text == "==" || currentToken.text == "!=")) {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseComparison());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparseFactor() {
    std::unique_ptr<ASTNode> left(infixparsePrimary());

    while (currentToken.type == TokenType::OPERATOR &&
        (currentToken.text == "*" || currentToken.text == "/" || currentToken.text == "%")) {
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparsePrimary());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }

    return left.release();
}

ASTNode* infixParser::infixparsePrimary() {
    if (currentToken.type == TokenType::NUMBER) {
        double value = std::stod(currentToken.text);
        nextToken();
        if (currentToken.type == TokenType::OPERATOR && currentToken.text == "=") {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        return std::make_unique<Number>(value).release();
    }
    else if (currentToken.type == TokenType::BOOLEAN) {
        if (currentToken.text == "true") {
            nextToken();
            return new BooleanNode(true);
        }
        else if (currentToken.text == "false") {
            nextToken();
            return new BooleanNode(false);
        }
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    else if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.text;
        nextToken();
        if (currentToken.type == TokenType::OPERATOR && currentToken.text == "=") {
            nextToken();
            std::unique_ptr<ASTNode> expr(infixparseLogicalOr());
            return std::make_unique<Assignment>(varName, expr.release()).release();
        }
                return std::make_unique<Variable>(varName).release();
    }
    else if (currentToken.type == TokenType::OPERATOR && currentToken.text == "(") {
        nextToken();
        std::unique_ptr<ASTNode> expr(infixparseLogicalOr());
        if (currentToken.type == TokenType::OPERATOR && currentToken.text == ")") {
            nextToken();
            return expr.release();
        }
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    else if (currentToken.type == TokenType::OPERATOR && currentToken.text == "-") {
        nextToken();
        std::unique_ptr<ASTNode> expr(infixparsePrimary());
        return std::make_unique<BinaryOperation>("-", new Number(0.0), expr.release()).release();
    }
    else {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
}

Token infixParser::PeekNextToken() {
    if (index < tokens.size() - 1) {
        return tokens[index + 1];
    }
    return Token(0, 0, "END", TokenType::OPERATOR);
}

