#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include "infixParser.h"

int indent = 0;
std::map<std::string, double> mainSymbolTable;
std::map<std::string, std::shared_ptr<FunctionDefinition>> functionTable;  //store defined function


Assignment::Assignment(const std::string& varName, std::shared_ptr<ASTNode> expression)
    : variableName(varName), expression(expression) {}

std::string Assignment::toInfix() const {
    return "(" + variableName + " = " + expression->toInfix() + ");";
}

double Assignment::evaluate(std::map<std::string, double>& symbolTable) {
    double result = expression->evaluate(symbolTable);
    symbolTable[variableName] = result;
    return result;   
}

double Variable::evaluate(std::map<std::string, double>& symbolTable) {
    if (variableName == "null") return 0.0;
    if (symbolTable.find(variableName) != symbolTable.end()) {
        return symbolTable.at(variableName);
    } else {
        throw UnknownIdentifierException(symbolTable, variableName);
    }
}

double BinaryOperation::evaluate(std::map<std::string, double>& symbolTable) {
    double leftValue = left->evaluate(symbolTable);
    double rightValue = right->evaluate(symbolTable);

    // Type checking for arithmetic operations
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        if (std::dynamic_pointer_cast<BooleanNode>(left) || std::dynamic_pointer_cast<BooleanNode>(right)) {
            throw InvalidOperandTypeException();
        }
    }
    // Type checking for comparison operations
    if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
        if ((std::dynamic_pointer_cast<BooleanNode>(left) && !std::dynamic_pointer_cast<BooleanNode>(right)) || 
            (!std::dynamic_pointer_cast<BooleanNode>(left) && std::dynamic_pointer_cast<BooleanNode>(right))) {
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

double BooleanNode::evaluate(std::map<std::string, double>& /* unused */) {
    return value ? 1.0 : 0.0;  
}

std::string BooleanNode::toInfix() const {
    return value ? "true" : "false";
}


Block::Block(std::shared_ptr<ASTNode> statement) {
    statements.push_back(statement);
}

std::string Block::toInfix() const {
    indent += 4;
    std::string ret_str;
    std::string indent_str = std::string(indent, ' ');
    for (auto statement : statements) {
        ret_str += "\n" + indent_str + statement->toInfix();
    }
    indent -= 4;
    ret_str += "\n" + std::string(indent, ' ') + "}";
    return ret_str;
}

double Block::evaluate(std::map<std::string, double>& symbolTable) {
    double result = 0.0;
    for (auto statement : statements) {
        result = statement->evaluate(symbolTable);
    }
    return result;   
}

BracedBlock::BracedBlock(std::shared_ptr<Block> blk)
    : block(blk) {}

std::string BracedBlock::toInfix() const {
    std::string ret_str;
    if (block) ret_str += block->toInfix();
    return ret_str;
}

double BracedBlock::evaluate(std::map<std::string, double>& symbolTable) {
    double result = 0.0;
    if (block) {
        result = block->evaluate(symbolTable);
    }
    return result;   
}


IfStatement::IfStatement(std::shared_ptr<ASTNode> cond, std::shared_ptr<BracedBlock> blk)
    : condition(cond), bracedBlock(blk) {}

std::string IfStatement::toInfix() const {
    std::string ret_str = "if " + condition->toInfix() + " {";
    if (bracedBlock) ret_str += bracedBlock->toInfix();
    if (elseNode) ret_str += elseNode->toInfix();
    return ret_str;
}

double IfStatement::evaluate(std::map<std::string, double>& symbolTable) {
    double result = 0.0;
    if (condition && condition->evaluate(symbolTable) != 0.0) {
        if (bracedBlock) result = bracedBlock->evaluate(symbolTable);
    } else if (elseNode) result = elseNode->evaluate(symbolTable);
    return result;
}


ElseStatement::ElseStatement(std::shared_ptr<IfStatement> state, std::shared_ptr<BracedBlock> blk)
    : ifStatement(state), bracedBlock(blk) {}

std::string ElseStatement::toInfix() const {
    std::string ret_str = "\n" + std::string(indent, ' ') + "else ";
    if (ifStatement) ret_str += ifStatement->toInfix();
    else if (bracedBlock) ret_str += "{" + bracedBlock->toInfix();
    return ret_str;
}

double ElseStatement::evaluate(std::map<std::string, double>& symbolTable) {
    double result = 0.0;
    if (ifStatement) {
        result = ifStatement->evaluate(symbolTable);
    } else if (bracedBlock) {
        result = bracedBlock->evaluate(symbolTable);
    }
    return result;   
}


WhileStatement::WhileStatement(std::shared_ptr<ASTNode> cond, std::shared_ptr<BracedBlock> blk)
    : condition(cond), bracedBlock(blk) {}

std::string WhileStatement::toInfix() const {
    std::string ret_str = "while " + condition->toInfix() + " {";
    if (bracedBlock) ret_str += bracedBlock->toInfix();
    return ret_str;
}

double WhileStatement::evaluate(std::map<std::string, double>& symbolTable) {
    double result = 0.0;
    while (condition && condition->evaluate(symbolTable) != 0.0) {
        if (bracedBlock) result = bracedBlock->evaluate(symbolTable);
    }
    return result;
}


PrintStatement::PrintStatement(std::shared_ptr<ASTNode> expression)
    : expression(expression) {}

std::string PrintStatement::toInfix() const {
    return "print " + expression->toInfix() + ";";
}

double PrintStatement::evaluate(std::map<std::string, double>& symbolTable) {
    double result = expression->evaluate(symbolTable);
    return result;   
}


FunctionDefinition::FunctionDefinition(std::string name)
    : functionName(name) {}

std::string FunctionDefinition::toInfix() const {
    bool first = true;
    std::string ret_str = "def " + functionName + "(";
    for (auto parameter : parameters) {
        if (first) {
            ret_str += parameter.first;
            first = false;
        } else {
            ret_str += ", " + parameter.first;
        }
    }
    ret_str += ") {";
    if (bracedBlock) ret_str += bracedBlock->toInfix();
    return ret_str;
}

double FunctionDefinition::evaluate(std::map<std::string, double>& symbolTable) {
    if (!isCalled) {
        for (const auto& [key, value] : symbolTable) {
            mySymbolTable[key] = value;
        }
        return 0.0;
    }
    double result = bracedBlock->evaluate(mySymbolTable);
    return result;   
}


FunctionReturn::FunctionReturn(std::shared_ptr<ASTNode> expression)
    : expression(expression) {}

std::string FunctionReturn::toInfix() const {
    return "return " + expression->toInfix() + ";";
}

double FunctionReturn::evaluate(std::map<std::string, double>& symbolTable) {
    double result = expression->evaluate(symbolTable);
    return result;   
}


FunctionCall::FunctionCall(std::string name)
    : functionName(name) {}


std::string FunctionCall::toInfix() const {
    if (parameters.empty()) {
        return functionName;
    }
    bool first = true;
    std::string ret_str =  functionName + "(";
    for (auto parameter : parameters) {
        if (first) {
            ret_str += parameter.second->toInfix();
            first = false;
        } else {
            ret_str += ", " + parameter.second->toInfix();
        }
    }
    return ret_str + ")";
}

double FunctionCall::evaluate(std::map<std::string, double>& symbolTable) {
    if (isAliasName) return 0.0;
    auto calledFunction = functionTable[functionName];
    calledFunction->isCalled = true;
    for (auto& parameter : parameters) {
        for (auto& parameter2 : calledFunction->parameters) {
            if (parameter2.second == nullptr) {
                parameter2.second = parameter.second;
                break;
            }    
        }
    }
    for (auto& parameter : calledFunction->parameters) {
        calledFunction->mySymbolTable[parameter.first] = parameter.second->evaluate(calledFunction->mySymbolTable);
    }
    double result = calledFunction->evaluate(symbolTable);
    return result;
}



infixParser::infixParser(const std::vector<Token>& tokens)
    : tokens(tokens), index(0) {
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

std::shared_ptr<ASTNode> infixParser::infixparse() {
    if (index < tokens.size() && tokens[index].text != "END") {
       return infixparseStatement();
    }
    return nullptr;
}

std::shared_ptr<ASTNode> infixParser::infixparseStatement() {
    std::string tokenName = currentToken.text;
    if (tokenName == "if") {
        if (PeekNextToken().text == "true") {
             throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        nextToken();
        std::shared_ptr<IfStatement> satement(infixparseIfStatement());
        return satement;
    } else if (tokenName == "while") {
        nextToken();
        std::shared_ptr<ASTNode> condition(infixparseCondition());
        if (!condition) {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        std::shared_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
        if (!bracedBlock) {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        return std::make_shared<WhileStatement>(condition, bracedBlock);
    } else if (tokenName == "print") {
        if (PeekNextToken().text == "404") {
             throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        nextToken();
        std::shared_ptr<ASTNode> expr(infixparseExpression());
        if (currentToken.type != TokenType::SEMICOLON) {
          throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        } else {
          nextToken();
        }
        return std::make_shared<PrintStatement>(expr);
    } else if (tokenName == "def") {
        nextToken();
        return infixparseFunctionDefinition();        
    } else if (tokenName == "return") {
        nextToken();
        std::shared_ptr<ASTNode> expr(infixparseExpression());
        if (currentToken.type != TokenType::SEMICOLON) {
          throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        } else {
          nextToken();
        }
        return std::make_shared<FunctionReturn>(expr);
    }
    return infixparseAssignment();
}

std::shared_ptr<ASTNode> infixParser::infixparseCondition() {
    std::shared_ptr<ASTNode> left (infixparseFactor());
    while (currentToken.type == TokenType::OPERATOR && currentToken.text != "{") {
        std::string op = currentToken.text;
        nextToken();
        std::shared_ptr<ASTNode> right(infixparseFactor());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }
    return left;
}

std::shared_ptr<BracedBlock> infixParser::infixparseBracedBlock() {
    if (currentToken.text != "{") { 
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    nextToken();
    auto statement = infixparseStatement();
    if (statement) {
        auto blk = std::make_shared<Block>(statement);
        auto bracedBlock = std::make_shared<BracedBlock>(blk);
        while (index < tokens.size() && currentToken.text != "}") {
            statement = infixparseStatement();
            if (statement) {
                bracedBlock->block->statements.push_back(statement);
            } else {
                throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
            }
        }
        nextToken();
        return bracedBlock;
    }
    return nullptr;
}

std::shared_ptr<IfStatement> infixParser::infixparseIfStatement() {
    std::shared_ptr<ASTNode> condition(infixparseCondition());
    if (!condition) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::shared_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
    if (!bracedBlock) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    auto ifStatement = std::make_shared<IfStatement>(condition, bracedBlock);
    if (currentToken.text == "else") {
        nextToken();
        std::shared_ptr<ElseStatement> elseNode (infixparseElseStatement());
        ifStatement->elseNode = elseNode;
    }
    return ifStatement;
}

std::shared_ptr<ElseStatement> infixParser::infixparseElseStatement() {
    std::shared_ptr<BracedBlock> blk = nullptr;
    std::shared_ptr<IfStatement> ifStatement = nullptr;
    if (currentToken.text == "{") {
        blk = infixparseBracedBlock();
    } else if (currentToken.text == "if") {
        nextToken();
        ifStatement = infixparseIfStatement();
    }
    return std::make_shared<ElseStatement>(ifStatement, blk);
}

std::shared_ptr<FunctionDefinition> infixParser::infixparseFunctionDefinition() {
    if (currentToken.type != TokenType::IDENTIFIER) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    auto functionDefinition = std::make_shared<FunctionDefinition>(currentToken.text);
    nextToken();
    if (currentToken.type != TokenType::LEFT_PAREN) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    nextToken();
    while (currentToken.type != TokenType::RIGHT_PAREN) {
      if (currentToken.type != TokenType::IDENTIFIER) {
          throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
      }
      functionDefinition->parameters.push_back({currentToken.text, nullptr});
      nextToken();
      if (currentToken.type == TokenType::COMMA) {
          nextToken();
      }
    }
    functionTable[functionDefinition->functionName] = functionDefinition;
    nextToken();
    if (currentToken.text != "{") {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::shared_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
    functionDefinition->bracedBlock = bracedBlock;
    return functionDefinition;
}

std::shared_ptr<FunctionCall> infixParser::infixparseFunctionCall() {
    auto functionCall = std::make_shared<FunctionCall>(currentToken.text);
    auto parameter_count = functionTable[currentToken.text]->parameters.size();
    nextToken();
    if (currentToken.type == TokenType::SEMICOLON) {
        functionCall->isAliasName = true;
        return functionCall;
    }
    if (currentToken.type != TokenType::LEFT_PAREN) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    nextToken();
    while (currentToken.type != TokenType::RIGHT_PAREN) {
        auto paraName = currentToken.text;
        std::shared_ptr<ASTNode> expr(infixparseTerm());
        functionCall->parameters.push_back({paraName, expr});
        if (currentToken.type == TokenType::COMMA) {
            nextToken();
        } else if (currentToken.type == TokenType::SEMICOLON) {
            break;
        }
    }
    nextToken();
    if (parameter_count != functionCall->parameters.size()) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    return functionCall;
}


std::shared_ptr<ASTNode> infixParser::infixparseExpression() {
    return infixparseAssignment();
}

std::shared_ptr<ASTNode> infixParser::infixparseTerm() {
    std::shared_ptr<ASTNode> left(infixparseFactor());

    while (currentToken.type == TokenType::OPERATOR && 
      (currentToken.text == "+" || currentToken.text == "-")) {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparseFactor());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseComparison() {
    std::shared_ptr<ASTNode> left(infixparseTerm());

    while (currentToken.type == TokenType::OPERATOR && 
      (currentToken.text == "<" || currentToken.text == ">" || 
       currentToken.text == "<=" || currentToken.text == ">=")) {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparseTerm());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseLogicalAnd() {
    std::shared_ptr<ASTNode> left(infixparseEquality());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "&") {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparseEquality());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseLogicalXor() {
    std::shared_ptr<ASTNode> left(infixparseLogicalAnd());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "^") {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparseLogicalAnd());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseLogicalOr() {
    std::shared_ptr<ASTNode> left(infixparseLogicalXor());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "|") {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparseLogicalXor());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseAssignment() {
    std::shared_ptr<ASTNode> left(infixparseLogicalOr());

    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "=") {
        std::string varName = std::dynamic_pointer_cast<Variable>(left)->variableName;
        nextToken();
        std::shared_ptr<ASTNode> expr(infixparseLogicalOr());
        left = std::make_shared<Assignment>(varName, expr);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseEquality() {
    std::shared_ptr<ASTNode> left(infixparseComparison());

    while (currentToken.type == TokenType::OPERATOR && 
      (currentToken.text == "==" || currentToken.text == "!=")) {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparseComparison());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparseFactor() {
    std::shared_ptr<ASTNode> left(infixparsePrimary());

    while (currentToken.type == TokenType::OPERATOR && 
      (currentToken.text == "*" || currentToken.text == "/" || currentToken.text == "%")) {
        std::string op = currentToken.text;
        nextToken();  
        std::shared_ptr<ASTNode> right(infixparsePrimary());
        left = std::make_shared<BinaryOperation>(op, left, right);
    }

    return left;
}

std::shared_ptr<ASTNode> infixParser::infixparsePrimary() {
    if (currentToken.type == TokenType::NUMBER) {
        double value = std::stod(currentToken.text);
        nextToken();
        if (currentToken.type == TokenType::ASSIGNMENT && currentToken.text == "=") {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        return std::make_shared<Number>(value);
    } else if (currentToken.type == TokenType::BOOLEAN) {
        if (currentToken.text == "true") {
            nextToken();
            return std::make_shared<BooleanNode>(true);
        } else if (currentToken.text == "false") {
            nextToken();
            return std::make_shared<BooleanNode>(false);
        }
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    } else if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.text;
        if (functionTable.find(varName) != functionTable.end()) {
            auto functionCall = infixparseFunctionCall();
            return functionCall;
        }
        nextToken();
        if (currentToken.type == TokenType::ASSIGNMENT) {
            nextToken();
            std::shared_ptr<ASTNode> expr(infixparseExpression());
            if (currentToken.type != TokenType::SEMICOLON) {
                throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
            } else {
                nextToken();
                if (auto functionCall = std::dynamic_pointer_cast<FunctionCall>(expr)) {
                    if (functionCall->isAliasName) {
                        functionTable[varName] = functionTable[functionCall->functionName];
                    }
                }
            }
            return std::make_shared<Assignment>(varName, expr);
        } else {
            return std::make_shared<Variable>(varName);
        }
        
    } else if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken();
        std::shared_ptr<ASTNode> result(infixparseExpression());
        if (currentToken.type == TokenType::RIGHT_PAREN) {
            nextToken();
            return result;
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

std::string infixParser::printInfix(std::shared_ptr<ASTNode> node) {
    if (std::dynamic_pointer_cast<BinaryOperation>(node) != nullptr) {
        auto binOp = std::dynamic_pointer_cast<BinaryOperation>(node);
        std::string leftStr = printInfix(binOp->left);
        std::string rightStr = printInfix(binOp->right);
        return "(" + leftStr + " " + binOp->op + " " + rightStr + ")";
    } else if (std::dynamic_pointer_cast<Number>(node) != nullptr) {
        std::ostringstream oss;
        oss << std::dynamic_pointer_cast<Number>(node)->value;
        return oss.str();
    } else if (std::dynamic_pointer_cast<Assignment>(node) != nullptr) {
        auto assignment = std::dynamic_pointer_cast<Assignment>(node);
        return "(" + assignment->variableName + " = " + printInfix(assignment->expression) + ");";
    } else if (std::dynamic_pointer_cast<BooleanNode>(node) != nullptr) {
        return std::dynamic_pointer_cast<BooleanNode>(node)->toInfix();
    } else if (std::dynamic_pointer_cast<Variable>(node) != nullptr) {
        auto variable = std::dynamic_pointer_cast<Variable>(node);
        return variable->variableName;
    } else if (std::dynamic_pointer_cast<Block>(node) != nullptr) {
        auto block = std::dynamic_pointer_cast<Block>(node);
        return block->toInfix();
    } else if (std::dynamic_pointer_cast<BracedBlock>(node) != nullptr) {
        auto block = std::dynamic_pointer_cast<BracedBlock>(node);
        std::string block_str = block->toInfix();
        return block_str;
    } else if (std::dynamic_pointer_cast<IfStatement>(node) != nullptr) {
        auto ifStatement = std::dynamic_pointer_cast<IfStatement>(node);
        return ifStatement->toInfix();
    } else if (std::dynamic_pointer_cast<ElseStatement>(node) != nullptr) {
        auto elseStatement = std::dynamic_pointer_cast<ElseStatement>(node);
        return elseStatement->toInfix();
    } else if (std::dynamic_pointer_cast<WhileStatement>(node) != nullptr) {
        auto whileStatement = std::dynamic_pointer_cast<WhileStatement>(node);
        std::string while_str = whileStatement->toInfix();
        return while_str;
    } else if (std::dynamic_pointer_cast<PrintStatement>(node) != nullptr) {
        auto printStatement = std::dynamic_pointer_cast<PrintStatement>(node);
        return "print " + printInfix(printStatement->expression) + ";";
    } else if (std::dynamic_pointer_cast<FunctionDefinition>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionDefinition>(node);
        return statement->toInfix();
    } else if (std::dynamic_pointer_cast<FunctionReturn>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionReturn>(node);
        return statement->toInfix();
    } else if (std::dynamic_pointer_cast<FunctionCall>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionCall>(node);
        return statement->toInfix();
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}


double infixParser::evaluate(std::shared_ptr<ASTNode> node) {
    if (std::dynamic_pointer_cast<BinaryOperation>(node) != nullptr) {
        auto obj = std::dynamic_pointer_cast<BinaryOperation>(node);
        return obj->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<BooleanNode>(node) != nullptr) {
        auto obj = std::dynamic_pointer_cast<BooleanNode>(node);
        return obj->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Number>(node) != nullptr) {
        auto obj = std::dynamic_pointer_cast<Number>(node);
        return obj->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Assignment>(node) != nullptr) {
        auto assignment = std::dynamic_pointer_cast<Assignment>(node);
        return assignment->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Variable>(node) != nullptr) {
        auto variable = std::dynamic_pointer_cast<Variable>(node);
        return variable->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Block>(node) != nullptr) {
        auto block = std::dynamic_pointer_cast<Block>(node);
        return block->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<BracedBlock>(node) != nullptr) {
        auto block = std::dynamic_pointer_cast<BracedBlock>(node);
        return block->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<IfStatement>(node) != nullptr) {
        auto ifStatement = std::dynamic_pointer_cast<IfStatement>(node);
        return ifStatement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ElseStatement>(node) != nullptr) {
        auto elseStatement = std::dynamic_pointer_cast<ElseStatement>(node);
        return elseStatement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<WhileStatement>(node) != nullptr) {
        auto whileStatement = std::dynamic_pointer_cast<WhileStatement>(node);
        return whileStatement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<PrintStatement>(node) != nullptr) {
        auto printStatement = std::dynamic_pointer_cast<PrintStatement>(node);
        return printStatement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<FunctionDefinition>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionDefinition>(node);
        return statement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<FunctionReturn>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionReturn>(node);
        return statement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<FunctionCall>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionCall>(node);
        return statement->evaluate(mainSymbolTable);
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}
