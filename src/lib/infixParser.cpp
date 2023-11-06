#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include "infixParser.h"


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
    
    // Type checking for arithmetic operations
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        if (dynamic_cast<BooleanNode*>(left) || dynamic_cast<BooleanNode*>(right)) {
            throw InvalidOperandTypeException();
        }
    }
    // Type checking for comparison operations
    if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
        if ((dynamic_cast<BooleanNode*>(left) && !dynamic_cast<BooleanNode*>(right)) || 
            (!dynamic_cast<BooleanNode*>(left) && dynamic_cast<BooleanNode*>(right))) {
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
        if (currentToken.type == TokenType::ASSIGNMENT && currentToken.text == "=") {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        return std::make_unique<Number>(value).release();
    } else if (currentToken.type == TokenType::BOOLEAN) {
        if (currentToken.text == "true") {
            nextToken();
            return new BooleanNode(true);
        } else if (currentToken.text == "false") {
            nextToken();
            return new BooleanNode(false);
        }
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
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
        throw UnknownIdentifierException(symbolTable,currentToken.text);
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
    } else if (dynamic_cast<BooleanNode*>(node) != nullptr) {
        return dynamic_cast<BooleanNode*>(node)->toInfix();
    } else if (dynamic_cast<Variable*>(node) != nullptr) {
        Variable* variable = dynamic_cast<Variable*>(node);
        return variable->variableName;
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}