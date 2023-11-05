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

IfStatement::IfStatement(ASTNode* expression)
    : expression(expression) {}

WhileStatement::WhileStatement(ASTNode* expression)
    : expression(expression) {}
    
PrintStatement::PrintStatement(ASTNode* expression)
    : expression(expression) {}

double IfStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = expression->evaluate(symbolTable);
    return result;   
}

double WhileStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = expression->evaluate(symbolTable);
    return result;   
}

double PrintStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = expression->evaluate(symbolTable);
    return result;   
}

std::string IfStatement::toInfix() const {
    return "if (" + expression->toInfix() + ") {";
}

std::string WhileStatement::toInfix() const {
    return "while (" + expression->toInfix() + ") {";
}

std::string PrintStatement::toInfix() const {
    return "print " + expression->toInfix();
}

double BinaryOperation::evaluate(std::map<std::string, double>& symbolTable) const {
    double leftValue = left->evaluate(symbolTable);
    double rightValue = right->evaluate(symbolTable);

    if (op == "+") return leftValue + rightValue;
    if (op == "-") return leftValue - rightValue;
    if (op == "*") return leftValue * rightValue;
    if (op == "/") {
        if (rightValue == 0) {
            throw DivisionByZeroException();
        }
        return leftValue / rightValue;
    }
    if (op == "%") return std::fmod(leftValue, rightValue);
    if (op == "<") return leftValue < rightValue ? 1 : 0;
    if (op == ">") return leftValue > rightValue ? 1 : 0;
    if (op == "<=") return leftValue <= rightValue ? 1 : 0;
    if (op == ">=") return leftValue >= rightValue ? 1 : 0;
    if (op == "==") return leftValue == rightValue ? 1 : 0;
    if (op == "!=") return leftValue != rightValue ? 1 : 0;
    if (op == "&") return static_cast<int>(leftValue) & static_cast<int>(rightValue);
    if (op == "^") return static_cast<int>(leftValue) ^ static_cast<int>(rightValue);
    if (op == "|") return static_cast<int>(leftValue) | static_cast<int>(rightValue);
std::cout << "Daisy BinaryOperation::evaluate() 2" << std::endl;
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
std::cout << "Daisy infixParser::nextToken() 2" << tokens.size() << std::endl;
        currentToken = Token(0, 0, "END", TokenType::OPERATOR);
    }
}

std::vector<ASTNode*> infixParser::infixparse() {
    while (index < tokens.size() && tokens[index].text != "END") {
       auto root = infixparseStatement();
       roots.push_back(root);
    }
    return roots;
}

ASTNode* infixParser::infixparseStatement() {
    std::string tokenName = currentToken.text;
std::cout << "Daisy infixParser::infixparseStatement() tokens size: " << index << " : " << tokenName << std::endl;
    if  (tokens.size() == 1 && tokenName == "END") {
        return std::make_unique<EmptyStatement>().release();
    }
    if (tokenName == "if") {
        nextToken();
        std::unique_ptr<ASTNode> left (infixparseFactor());
        while (currentToken.type == TokenType::OPERATOR && currentToken.text != "{") {
            std::string op = currentToken.text;
            nextToken();  
            std::unique_ptr<ASTNode> right(infixparseFactor());
            left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
        }
        nextToken();
        return std::make_unique<IfStatement>(left.release()).release();
    } else if (tokenName == "while") {
        nextToken();
        std::unique_ptr<ASTNode> left (infixparseFactor());
        std::string op = currentToken.text;
        nextToken();
        std::unique_ptr<ASTNode> right(infixparseFactor());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
        nextToken();
        return std::make_unique<WhileStatement>(left.release()).release();
    } else if (tokenName == "print") {
        nextToken();
        std::unique_ptr<ASTNode> expr(infixparseExpression());        
        return std::make_unique<PrintStatement>(expr.release()).release();
    } else if (tokenName == "else") {
        nextToken();
        nextToken();
        return std::make_unique<ElseStatement>().release();
    } else if (tokenName == "}") {
        nextToken();
        return std::make_unique<EndStatement>().release();
    }
    return infixparseExpression();
}

ASTNode* infixParser::infixparseExpression() {
    std::unique_ptr<ASTNode> left(infixparseTerm());

    while (currentToken.type == TokenType::OPERATOR && 
      (currentToken.text == "+" || currentToken.text == "-" || 
       currentToken.text == "<" || currentToken.text == ">" || 
       currentToken.text == "<=" || currentToken.text == ">=" || 
       currentToken.text == "==" || currentToken.text == "!=")) {
        std::string op = currentToken.text;
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
    std::unique_ptr<ASTNode> left (infixparseFactor());

    while (currentToken.type == TokenType::OPERATOR && 
      (currentToken.text == "*" || currentToken.text == "/" || 
       currentToken.text == "%" || currentToken.text == "&" || 
       currentToken.text == "^" || currentToken.text == "|")) {
        std::string op = currentToken.text;
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
        if (currentToken.type == TokenType::ASSIGNMENT && currentToken.text == "=") {
std::cout << "Daisy infixParser::infixparsePrimary() 1  " << currentToken.line << " : " << currentToken.column << " : " << currentToken.text << std::endl;
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
std::cout << "Daisy infixParser::infixparsePrimary() 2  " << currentToken.line << " : " << currentToken.column << " : " << currentToken.text << std::endl;
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
std::cout << "Daisy infixParser::infixparsePrimary() 3  " << currentToken.line << " : " << currentToken.column << " : " << currentToken.text << std::endl;
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
    } else if (currentToken.type == TokenType::RIGHT_PAREN) {
std::cout << "Daisy infixParser::infixparsePrimary() 4  " << currentToken.line << " : " << currentToken.column << " : " << currentToken.text << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    } else {
std::cout << "Daisy infixParser::infixparsePrimary() 5  " << currentToken.line << " : " << currentToken.column << " : " << currentToken.text << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
}

Token infixParser::PeekNextToken() {
    if (index < tokens.size() - 1) {
        return tokens[index + 1];
    }
    return Token(0, 0, "END", TokenType::OPERATOR);
}

std::string infixParser::printInfix(ASTNode* node, bool root) {
    static int indent = 0;
    std::string indent_str;
    if (root) indent_str = std::string(indent, ' ');
    if (dynamic_cast<BinaryOperation*>(node) != nullptr) {
        BinaryOperation* binOp = dynamic_cast<BinaryOperation*>(node);
        std::string leftStr = printInfix(binOp->left, false);
        std::string rightStr = printInfix(binOp->right, false);
        return indent_str + "(" + leftStr + " " + binOp->op + " " + rightStr + ")";
    } else if (dynamic_cast<Number*>(node) != nullptr) {
        std::ostringstream oss;
        oss << dynamic_cast<Number*>(node)->value;
        return oss.str();
    } else if (dynamic_cast<Assignment*>(node) != nullptr) {
        Assignment* assignment = dynamic_cast<Assignment*>(node);
        return indent_str + "(" + assignment->variableName + " = " + printInfix(assignment->expression, false) + ")";
    } else if (dynamic_cast<Variable*>(node) != nullptr) {
        Variable* variable = dynamic_cast<Variable*>(node);
        return variable->variableName;
    } else if (dynamic_cast<IfStatement*>(node) != nullptr) {
        indent += 4;
        IfStatement* ifstatement = dynamic_cast<IfStatement*>(node);
        return indent_str + "if " + printInfix(ifstatement->expression, false) + " {";
    } else if (dynamic_cast<WhileStatement*>(node) != nullptr) {
        indent += 4;
        WhileStatement* whileStatement = dynamic_cast<WhileStatement*>(node);
        return indent_str + "while " + printInfix(whileStatement->expression, false) + " {";
    } else if (dynamic_cast<PrintStatement*>(node) != nullptr) {
        indent += 4;
        PrintStatement* printStatement = dynamic_cast<PrintStatement*>(node);
        return indent_str + "print " + printInfix(printStatement->expression, false);
    } else if (dynamic_cast<ElseStatement*>(node) != nullptr) {
        indent += 4;
        return indent_str + "else {";
    } else if (dynamic_cast<EndStatement*>(node) != nullptr) {
        indent -= 4;
        if (root) indent_str = std::string(indent, ' ');
        return indent_str + "}";
    } else if (dynamic_cast<EmptyStatement*>(node) != nullptr) {
        return indent_str;
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}

double BooleanNode::evaluate(std::map<std::string, double>& /* unused */) const {
    return value ? 1.0 : 0.0;  
}

double ElseStatement::evaluate(std::map<std::string, double>& /* unused */) const {
    return 0.0;  
}

double EndStatement::evaluate(std::map<std::string, double>& /* unused */) const {
    return 0.0;  
}

double EmptyStatement::evaluate(std::map<std::string, double>& /* unused */) const {
    return 0.0;  
}