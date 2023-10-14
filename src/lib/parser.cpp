#include "parser.h"
#include <iostream>

double BinaryOperation::evaluate() const {
    double leftValue = left->evaluate();
    double rightValue = right->evaluate();

    switch (op) {
        case '+': return leftValue + rightValue;
        case '-': return leftValue - rightValue;
        case '*': return leftValue * rightValue;
        case '/':
            if (rightValue == 0) {
                std::cerr << "Runtime error: division by zero." << std::endl;
                exit(3);
            }
            return leftValue / rightValue;
        default:
            std::cerr << "Invalid operator" << std::endl;
            exit(1);
    }
}

std::string BinaryOperation::toInfix() const {
    std::string leftStr = left->toInfix();
    std::string rightStr = right->toInfix();
    return "(" + leftStr + " " + op + " " + rightStr + ")";
}

std::string Number::toInfix() const {
    return std::to_string(value);
}

ASTNode* Parser::parse() {
    nextToken();
    ASTNode* result = parseExpression();
    if (currentToken.type != TokenType::END) { //ASK ABOUT END
        std::cerr << "Unexpected token at line " <<
            lexer.line << " column " <<
            lexer.column << ": " <<
            currentToken.text << std::endl;
        exit(1);
    }
    return result;
}

void Parser::nextToken() {
    currentToken = lexer.nextToken();
}

ASTNode* Parser::parsePrimary() {
    if (currentToken.type == TokenType::NUMBER) {
        double value = std::stod(currentToken.text);
        nextToken();
        return new Number(value);
    } else if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken();
        ASTNode* result = parseExpression();
        if (currentToken.type != TokenType::RIGHT_PAREN) {
            std::cerr << "Unexpected token at line " <<
                lexer.line << " column " <<
                lexer.column << ": " <<
                currentToken.text << std::endl;
            exit(1);
        }
        nextToken();
        return result;
    } else {
        std::cerr << "Unexpected token at line " <<
            lexer.line << " column " <<
            lexer.column << ": " <<
            currentToken.text << std::endl;
        exit(1);
    }
}

ASTNode* Parser::parseExpression() {
    ASTNode* left = parseTerm();
    while (currentToken.type == TokenType::OPERATOR) {
        char op = currentToken.text[0];
        nextToken();
        ASTNode* right = parseTerm();
        left = new BinaryOperation(op, left, right);
    }
    return left;
}

ASTNode* Parser::parseTerm() {
    ASTNode* left = parseFactor();
    while (currentToken.type == TokenType::OPERATOR) {
        char op = currentToken.text[0];
        nextToken();
        ASTNode* right = parseFactor();
        left = new BinaryOperation(op, left, right);
    }
    return left;
}

ASTNode* Parser::parseFactor() { //used to find important token for tree construction
    return parsePrimary();
}

std::string Parser::printInfix(ASTNode* node) {
    std::string result;
    if (dynamic_cast<BinaryOperation*>(node) != nullptr) {
        BinaryOperation* binOp = dynamic_cast<BinaryOperation*>(node);

        std::string leftStr = printInfix(binOp->left);
        std::string rightStr = printInfix(binOp->right);

        result = "(" + leftStr + " " + binOp->op + " " + rightStr + ")";
    } else if (dynamic_cast<Number*>(node) != nullptr) {
        result = std::to_string(dynamic_cast<Number*>(node)->evaluate());
    }

    // Add a newline character to separate the expression and result
    result += "\n";

    return result;
}


