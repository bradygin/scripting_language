#include <sstream>
#include "infixParser.h"

std::map<std::string, double> symbolTable;

Assignment::Assignment(const std::string& varName, ASTNode* expression)
    : variableName(varName), expression(expression) {}

double Assignment::evaluate() const {
    double result = expression->evaluate();
    
    if (symbolTable.find(variableName) != symbolTable.end()) {
        symbolTable[variableName] = result;
        return symbolTable[variableName];
    } else {
        std::cout << "Variable '" << variableName << "' not found." << std::endl;
        exit(2);
    }
}

std::string Assignment::toInfix() const {
    return "(" + variableName + " = " + expression->toInfix() + ")";
}

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
            std::cout << "Invalid operator" << std::endl;
            exit(2);
    }
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
        // when END is reached
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
        nextToken();  
        ASTNode* right = parseTerm();
        left = new BinaryOperation(op, left, right);
    }

    return left;
}


ASTNode* Parser::parseTerm() {
    ASTNode* left = parseFactor();

    while (currentToken.type == TokenType::OPERATOR && (currentToken.text == "*" || currentToken.text == "/")) {
        char op = currentToken.text[0];
        nextToken();  
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
        nextToken();
        return new Number(value);
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.text;
        nextToken();  

        if (currentToken.type == TokenType::ASSIGNMENT) {
            nextToken();  
            ASTNode* expr = parseExpression();

            // Store the variable value in the symbolTable
            symbolTable[varName] = expr->evaluate();

            return new Assignment(varName, expr);
        } else {
            if (symbolTable.find(varName) != symbolTable.end()) {
                return new Number(symbolTable[varName]);
            } else {
                std::cout << "Variable '" << varName << "' not found." << std::endl;
                exit(3);
            }
        }
    } else if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken();
        ASTNode* result = parseExpression();
        if (currentToken.type == TokenType::RIGHT_PAREN) {
            nextToken();
            return result;
        } else {
            std::cout << "Unexpected token at line " << currentToken.line
                      << " column " << currentToken.column << ": " << currentToken.text << std::endl;
            exit(2);
        }
    } else {
        std::cout << "Unexpected token at line " << currentToken.line
                  << " column " << currentToken.column << ": " << currentToken.text << std::endl;
        exit(2);
    }
}

std::string Parser::printInfix(ASTNode* node) {
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
