#include "parser.h"

// Implementation of BinaryOperation class methods
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
            exit(2);
    }
}

std::string BinaryOperation::toInfix() const {
    std::string leftStr = left->toInfix();
    std::string rightStr = right->toInfix();
    return "(" + leftStr + " " + op + " " + rightStr + ")";
}

// Implementation of Number class method
std::string Number::toInfix() const {
    return std::to_string(value);
}

// Implementation of Parser class methods
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), index(0) {
    if (!tokens.empty()) {
        currentToken = tokens[index];
    }
}

void Parser::nextToken() {
    if (index < tokens.size() - 1) {
        index++;
        currentToken = tokens[index];
    } else {
        // Handle end of tokens
        currentToken = Token(0, 0, "END", TokenType::OPERATOR);
    }
}

ASTNode* Parser::parse() {
    return parseExpression();
}

ASTNode* Parser::parseExpression() {
    ASTNode* left = parseTerm();

    while (currentToken.type == TokenType::OPERATOR && (currentToken.text == "+" || currentToken.text == "-")) {
        char op = currentToken.text[0];
        nextToken();  // Consume the operator token
        ASTNode* right = parseTerm();
        left = new BinaryOperation(op, left, right);
    }

    return left;
}

// ASTNode* Parser::parseExpression() {
//     if (currentToken.type == TokenType::LEFT_PAREN) {
//         nextToken();  // Consume the '(' token
//         ASTNode* left = parseExpression();
//         char op = currentToken.text[0]; // Operator (*, +, /, etc.)
//         nextToken();  // Consume the operator token
//         ASTNode* right = parseExpression();
//         if (currentToken.type == TokenType::RIGHT_PAREN) {
//             nextToken();  // Consume the ')' token
//             return new BinaryOperation(op, left, right);
//         } else {
//             std::cerr << "(2) Unexpected token at line " << currentToken.line
//                       << " column " << currentToken.column << ": " << currentToken.text << std::endl;
//             exit(2);
//         }
//     } else {
//         return parsePrimary();
//     }
// }

ASTNode* Parser::parseTerm() {
    ASTNode* left = parseFactor();

    while (currentToken.type == TokenType::OPERATOR && (currentToken.text == "*" || currentToken.text == "/")) {
        char op = currentToken.text[0];
        nextToken();  // Consume the operator token
        ASTNode* right = parseFactor();
        left = new BinaryOperation(op, left, right);
    }

    return left;
}

ASTNode* Parser::parseFactor() {
    return parsePrimary();
}

ASTNode* Parser::parsePrimary() {
    if (currentToken.type == TokenType::NUMBER) {
        double value = std::stod(currentToken.text);
        nextToken();  // Consume the number token
        return new Number(value);
    } else if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken();  // Consume the '(' token
        ASTNode* result = parseExpression();
        if (currentToken.type == TokenType::RIGHT_PAREN) {
            nextToken();  // Consume the ')' token
            return result;
        } else {
            std::cerr << "(2) Unexpected token at line " << currentToken.line
                      << " column " << currentToken.column << ": " << currentToken.text << std::endl;
            exit(2);
        }
    } else {
        std::cerr << "(3) Unexpected token at line " << currentToken.line
                  << " column " << currentToken.column << ": " << currentToken.text << std::endl;
        exit(2);
    }
}

// std::string Parser::printInfix(ASTNode* node) {
//     if (dynamic_cast<BinaryOperation*>(node) != nullptr) {
//         BinaryOperation* binOp = dynamic_cast<BinaryOperation*>(node);
//         std::string leftStr = printInfix(binOp->left);
//         std::string rightStr = printInfix(binOp->right);
//         return "(" + leftStr + " " + binOp->op + " " + rightStr + ")";
//     } else if (dynamic_cast<Number*>(node) != nullptr) {
//         return std::to_string(dynamic_cast<Number*>(node)->value);
//     } else {
//         std::cerr << "Invalid node type" << std::endl;
//         exit(4);
//     }
// }
 std::string Parser::printInfix(ASTNode* node) {
    if (dynamic_cast<BinaryOperation*>(node) != nullptr) {
        BinaryOperation* binOp = dynamic_cast<BinaryOperation*>(node);
        std::string leftStr = printInfix(binOp->left);
        std::string rightStr = printInfix(binOp->right);
        return "(" + leftStr + " " + binOp->op + " " + rightStr + ")";
    } else if (dynamic_cast<Number*>(node) != nullptr) {
        return std::to_string(dynamic_cast<Number*>(node)->value);
    } else {
        std::cerr << "Invalid node type" << std::endl;
        exit(4);
    }
}