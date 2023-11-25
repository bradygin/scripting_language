#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include "infixParser.h"

int indent = 0;
std::map<std::string, double> mainSymbolTable;
std::map<std::string, std::shared_ptr<FunctionDefinition>> functionTable;  //store defined function
std::map<std::string, std::shared_ptr<ArrayLiteral>> arrayTable;     //store defined array

Assignment::Assignment(const std::string& varName, std::shared_ptr<ASTNode> expression)
    : variableName(varName), expression(expression) {}

std::string Assignment::toInfix() const {
//std::cout << "Daisy Assignment::toInfix()  0   " << std::endl;
    return "(" + variableName + " = " + expression->toInfix() + ");";
}

double Assignment::evaluate(std::map<std::string, double>& symbolTable) {
//std::cout << "Daisy  Assignment::evaluate()  0 " << std::endl;
    double result = expression->evaluate(symbolTable);
    symbolTable[variableName] = result;
    return result;   
}

double Variable::evaluate(std::map<std::string, double>& symbolTable) {
//std::cout << "Daisy  Variable::evaluate()  0 variableName=" << variableName << std::endl;
    if (variableName == "null") return 0.0;
    if (symbolTable.find(variableName) != symbolTable.end()) {
//std::cout << "Daisy  Variable::evaluate()  0 symbolTable.at(variableName)=" << symbolTable.at(variableName) << std::endl;
        return symbolTable.at(variableName);
    } else {
        throw UnknownIdentifierException(symbolTable, variableName);
    }
}

double BinaryOperation::evaluate(std::map<std::string, double>& symbolTable) {
//std::cout << "Daisy  BinaryOperation::evaluate()  0 " << std::endl;
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
//std::cout << "Daisy  BooleanNode::evaluate()  0 " << std::endl;
    return value ? 1.0 : 0.0;  
}

std::string BooleanNode::toInfix() const {
    return value ? "true" : "false";
}


Block::Block(std::shared_ptr<ASTNode> statement) {
    statements.push_back(statement);
}

std::string Block::toInfix() const {
//std::cout << "Daisy Block::toInfix()  0   " << std::endl;
    indent += 4;
    std::string ret_str;
    std::string indent_str = std::string(indent, ' ');
    for (auto statement : statements) {
//std::cout << "Daisy Block::toInfix()  1   " << std::endl;
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
//std::cout << "Daisy BracedBlock::toInfix()  0   " << std::endl;
    std::string ret_str;
    if (block) ret_str += block->toInfix();
//std::cout << "Daisy BracedBlock::toInfix()  1   " << std::endl;
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
//std::cout << "Daisy  PrintStatement::evaluate()  0 " << std::endl;
    double result = expression->evaluate(symbolTable);
    return result;   
}


FunctionDefinition::FunctionDefinition(std::string name)
    : functionName(name) {}

std::string FunctionDefinition::toInfix() const {
    std::string ret_str = "def " + functionName + "(";
    bool first = true;
    for (auto parameter : parameters) {
        if (first) {
            ret_str += parameter.first;
            first = false;
        } else {
            ret_str += ", " + parameter.first;
        }
    }
    ret_str += ") {";
    if (bracedBlock) {
        ret_str += bracedBlock->toInfix();
    } else {
        ret_str += "\n}";
    }
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
    if (!expression) return "return;";
    return "return " + expression->toInfix() + ";";
}

double FunctionReturn::evaluate(std::map<std::string, double>& symbolTable) {
    double result = expression->evaluate(symbolTable);
    return result;   
}


FunctionCall::FunctionCall(std::string name)
    : functionName(name) {}


std::string FunctionCall::toInfix() const {
    if (isAliasName) {
        return functionName;
    }
    if (parameters.empty()) {
        return functionName + "()";
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


std::string ArrayLiteral::toInfix() const {
//std::cout << "Daisy  ArrayLiteral::toInfix()  0 " << std::endl;
    // Create a string representation of the array
    std::string result = "[";
    for (const auto& element : elements) {
        result += element->toInfix() + ", ";
    }
    // Remove the trailing ", " if there are elements
    if (!elements.empty()) {
        result.erase(result.length() - 2);
    }
    result += "]";
    return result;
}

double ArrayLiteral::evaluate(std::map<std::string, double>& symbolTable) {
//std::cout << "Daisy  ArrayLiteral::evaluate()  0 " << std::endl;
    // For simplicity, let's assume the array elements are numbers and sum them up
    double sum = 0.0;
    std::cout << "[" << std::endl;
    
    for (const auto& element : elements) {
        sum += element->evaluate(symbolTable);
    }
    return sum;
}

std::string ArrayLookup::toInfix() const {
//std::cout << "Daisy  ArrayLookup::toInfix()  0 " << std::endl;
    // Create a string representation of the array lookup
    return arrayName + "[" + index->toInfix() + "]";
}

double ArrayLookup::evaluate(std::map<std::string, double>& symbolTable) {
//std::cout << "Daisy  ArrayLookup::evaluate()  0 arrayName= " << arrayName << std::endl;
    // Check if the array and index are valid
    if (arrayTable.find(arrayName) == arrayTable.end()) {
        throw std::runtime_error("Invalid array type for lookup.");
    }
    if (!index) {
        throw std::runtime_error("index cannot be null.");
    }
    auto arrayLiteral = arrayTable[arrayName];
    // Evaluate index to get their values
    double indexValue = index->evaluate(symbolTable);
    // Check if the index is a valid integer
    if (std::floor(indexValue) != indexValue) {
        throw std::runtime_error("Array index must be an integer.");
    }
    int arrayIndex = static_cast<int>(indexValue);
//std::cout << "Daisy  ArrayLookup::evaluate()  1 arrayIndex= " << arrayIndex << std::endl;
    // Check if the array index is within bounds
    if (arrayIndex < 0 || arrayIndex >= static_cast<int>(arrayLiteral->elements.size())) {
        throw std::runtime_error("Array index out of bounds.");
    }
    // Use the ArrayLiteral class method to get the value at the specified index
    return arrayLiteral->elements[arrayIndex]->evaluate(symbolTable);
}

void ArrayLookup::setAssignmentValue(std::shared_ptr<ASTNode> value) {
//std::cout << "Daisy  ArrayLookup::setAssignmentValue()  0  arrayName= " << arrayName << std::endl;
    // Check if the array and index are valid
    if (arrayTable.find(arrayName) == arrayTable.end()) {
        throw std::runtime_error("Invalid array type for assignment.");
    }
    if (!index) {
        throw std::runtime_error("index cannot be null.");
    }
    auto arrayLiteral = arrayTable[arrayName];
    // Evaluate index to get their values
    double indexValue = index->evaluate(symbolTable);
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

std::string ArrayAssignment::toInfix() const {
//std::cout << "Daisy  ArrayAssignment::toInfix()  0 " << std::endl;
    // Create a string representation of the array lookup
    return "(" + arrayName + "[" + index->toInfix() + "] = " + asignment->toInfix() + ");";
}

double ArrayAssignment::evaluate(std::map<std::string, double>& symbolTable) {
//std::cout << "Daisy  ArrayAssignment::evaluate()  0 arrayName= " << arrayName << std::endl;
    // Check if the array and index are valid
    if (arrayTable.find(arrayName) == arrayTable.end()) {
        throw std::runtime_error("Invalid array type for assignment.");
    }
    if (!index) {
        throw std::runtime_error("index cannot be null.");
    }
    auto arrayLiteral = arrayTable[arrayName];
    // Evaluate index to get their values
    double indexValue = index->evaluate(symbolTable);
//std::cout << "Daisy  ArrayAssignment::evaluate()  1 indexValue= " << indexValue << std::endl;
    // Check if the index is a valid integer
    if (std::floor(indexValue) != indexValue) {
        throw std::runtime_error("Array index must be an integer.");
    }
    int arrayIndex = static_cast<int>(indexValue);
    // Check if the array index is within bounds
    if (arrayIndex < 0 || arrayIndex >= static_cast<int>(arrayLiteral->elements.size())) {
        throw std::runtime_error("Array index out of bounds.");
    }
    // Use the ArrayLiteral class method to get the value at the specified index
    return arrayLiteral->elements[arrayIndex]->evaluate(symbolTable);
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
//std::cout << "Daisy infixParser::infixparse()  0   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    if (index < tokens.size() && tokens[index].text != "END") {
       return infixparseStatement();
    }
    return nullptr;
}

std::shared_ptr<ASTNode> infixParser::infixparseStatement() {
//std::cout << "Daisy infixParser::infixparseStatement()  0   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    std::string tokenName = currentToken.text;
    if (tokenName == "if") {
        auto statement = infixparseIfStatement();
        return statement;
    } else if (tokenName == "while") {
        auto statement = infixparseWhileStatement();
        return statement;
    } else if (tokenName == "print") {
        auto statement = infixparsePrintStatement();
        return statement;
    } else if (tokenName == "def") {
        auto statement = infixparseFunctionDefinition();
        return statement;
    } else if (tokenName == "return") {
        auto statement = infixparseReturnStatement();
        return statement;
    } else if (tokenName == "[") {
        auto statement = infixparseArrayLiteral();
        return statement;
    }
//std::cout << "Daisy infixParser::infixparseStatement()  7   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    auto asignment = infixparseAssignment();
//std::cout << "Daisy infixParser::infixparseStatement()  8   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    return asignment;
}

std::shared_ptr<ASTNode> infixParser::infixparseCondition() {
//std::cout << "Daisy infixParser::infixparseCondition()  0   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    std::shared_ptr<ASTNode> left (infixparseFactor());
//std::cout << "Daisy infixParser::infixparseCondition()  1   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    while (currentToken.type == TokenType::OPERATOR && currentToken.text != "{") {
        std::string op = currentToken.text;
        nextToken();
//std::cout << "Daisy infixParser::infixparseCondition()  2   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
        std::shared_ptr<ASTNode> right(infixparseFactor());
//std::cout << "Daisy infixParser::infixparseCondition()  3   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
        left = std::make_shared<BinaryOperation>(op, left, right);
    }
    return left;
}

std::shared_ptr<BracedBlock> infixParser::infixparseBracedBlock() {
//std::cout << "Daisy infixParser::infixparseBracedBlock()  0   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    if (currentToken.text != "{") { 
//std::cout << "Daisy infixParser::infixparseBracedBlock()  1 " << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    nextToken();
//std::cout << "Daisy infixParser::infixparseBracedBlock()  2   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    auto statement = infixparseStatement();
//std::cout << "Daisy infixParser::infixparseBracedBlock()  3   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
    if (statement) {
//std::cout << "Daisy infixParser::infixparseBracedBlock()  4   " << currentToken.line << ":" << currentToken.column << " : " << currentToken.text << std::endl;
        auto blk = std::make_shared<Block>(statement);
        auto bracedBlock = std::make_shared<BracedBlock>(blk);
        while (index < tokens.size() && currentToken.text != "}") {
            statement = infixparseStatement();
            if (statement) {
                bracedBlock->block->statements.push_back(statement);
            } else {
//std::cout << "Daisy infixParser::infixparseBracedBlock()  2 " << std::endl;
                throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
            }
        }
        nextToken();
        return bracedBlock;
    }
    return nullptr;
}

std::shared_ptr<IfStatement> infixParser::infixparseIfStatement() {
    nextToken();
    if (currentToken.text == "true") {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::shared_ptr<ASTNode> condition(infixparseCondition());
    if (!condition) {
//std::cout << "Daisy infixParser::infixparseIfStatement()  5 " << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::shared_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
    if (!bracedBlock) {
//std::cout << "Daisy infixParser::infixparseIfStatement()  6 " << std::endl;
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

std::shared_ptr<WhileStatement> infixParser::infixparseWhileStatement() {
    nextToken();
    std::shared_ptr<ASTNode> condition(infixparseCondition());
    if (!condition) {
//std::cout << "Daisy infixParser::infixparseWhileStatement()  3 " << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::shared_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
    if (!bracedBlock) {
//std::cout << "Daisy infixParser::infixparseWhileStatement()  4 " << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    return std::make_shared<WhileStatement>(condition, bracedBlock);
}

std::shared_ptr<PrintStatement> infixParser::infixparsePrintStatement() {
    nextToken();
    if (currentToken.text == "404") {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::shared_ptr<ASTNode> expr(infixparseExpression());
    if (currentToken.type != TokenType::SEMICOLON) {
//std::cout << "Daisy infixParser::infixparsePrintStatement()  5 " << std::endl;
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    } else {
        nextToken();
    }
    return std::make_shared<PrintStatement>(expr);
}

std::shared_ptr<FunctionDefinition> infixParser::infixparseFunctionDefinition() {
    nextToken();
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
    if (PeekNextToken().text == "}") {
      nextToken();
      nextToken();
    } else {
        std::shared_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
        functionDefinition->bracedBlock = bracedBlock;
    }
    return functionDefinition;
}

std::shared_ptr<FunctionReturn> infixParser::infixparseReturnStatement() {
    nextToken();
    if (currentToken.type == TokenType::SEMICOLON) {
        nextToken();
        return std::make_shared<FunctionReturn>(nullptr);
    }
    std::shared_ptr<ASTNode> expr(infixparseExpression());
    if (currentToken.type != TokenType::SEMICOLON) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    } else {
        nextToken();
    }
    return std::make_shared<FunctionReturn>(expr);
}

std::shared_ptr<FunctionCall> infixParser::infixparseFunctionCall() {
    auto functionname = currentToken.text;
    auto functionCall = std::make_shared<FunctionCall>(functionname);    
    nextToken();
    if (currentToken.type == TokenType::SEMICOLON) {
        functionCall->isAliasName = true;
        return functionCall;
    }
    if (currentToken.type != TokenType::LEFT_PAREN) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    nextToken();
    if (currentToken.type == TokenType::RIGHT_PAREN) {
        nextToken();
        if (currentToken.type == TokenType::SEMICOLON) {
            nextToken();
        }
        return functionCall;
    }
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
    /*auto parameter_count = functionTable[functionname]->parameters.size();
    if (parameter_count != functionCall->parameters.size()) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }*/
    return functionCall;
}

// ADDED FUNCTIONALITY FOR ARRAYS
std::shared_ptr<ASTNode> infixParser::infixparseExpression() {
    auto asignment = infixparseAssignment();
    return asignment;
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
    if (currentToken.type == TokenType::IDENTIFIER) {
        std::string varName = currentToken.text;
        if (functionTable.find(varName) != functionTable.end()) {
            auto functionCall = infixparseFunctionCall();
            return functionCall;
        /*else if (arrayTable.find(varName) != arrayTable.end()) {
            auto arrayAssignment = infixparseArrayAssignment();
            return arrayAssignment;*/
        } else if (PeekNextToken().text == "(") {
            auto functionCall = infixparseFunctionCall();
            return functionCall;
        } else if (PeekNextToken().type == TokenType::LEFT_SQUARE) {
            nextToken();  // skip varName
            auto arrayLookup = infixparseArrayLookup(varName);
            if (currentToken.type == TokenType::ASSIGNMENT) {
                nextToken();
                auto arrayAssignment = std::make_shared<ArrayAssignment>(varName, arrayLookup->index);
                std::shared_ptr<ASTNode> asignment(infixparseAssignment());
                arrayAssignment->asignment = asignment;
                if (currentToken.type == TokenType::SEMICOLON) {
                    nextToken(); // Consume ';'
                }
                return arrayAssignment;
            } else {
                return arrayLookup;
            }
        } else if (PeekNextToken().type == TokenType::ASSIGNMENT) {
            nextToken();  // skip varName
            nextToken();  // skip =
            std::shared_ptr<ASTNode> asignment(infixparseAssignment());
            if (currentToken.type != TokenType::SEMICOLON) {
                throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
            } else {
                nextToken();
                if (auto functionCall = std::dynamic_pointer_cast<FunctionCall>(asignment)) {
                    if (functionCall->isAliasName) {
                        functionTable[varName] = functionTable[functionCall->functionName];
                    }
                } else if (auto arrayLiteral = std::dynamic_pointer_cast<ArrayLiteral>(asignment)) {
                    arrayTable[varName] = arrayLiteral;
                } else if (auto variable = std::dynamic_pointer_cast<Variable>(asignment)) {
                    if (arrayTable.find(variable->variableName) != arrayTable.end()) {
                        arrayTable[varName] = arrayTable[variable->variableName];
                    }
                }
            }
            return std::make_shared<Assignment>(varName, asignment);
        }
    } else if (currentToken.type == TokenType::LEFT_SQUARE) {
        return infixparseArrayLiteral();
    }
    std::shared_ptr<ASTNode> left(infixparseLogicalOr());
    
    while (currentToken.type == TokenType::OPERATOR && currentToken.text == "=") {
        std::string varName = std::dynamic_pointer_cast<Variable>(left)->variableName;
        nextToken();
        std::shared_ptr<ASTNode> expr(infixparseLogicalOr());
        left = std::make_shared<Assignment>(varName, expr);
    }
    if (currentToken.type == TokenType::ASSIGNMENT) {
        std::cout << "Runtime error: invalid assignee." << std::endl;
        exit(3);
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

// ADDED FUNCTIONALITY FOR ARRAYS
std::shared_ptr<ASTNode> infixParser::infixparsePrimary() {
    if (currentToken.type == TokenType::NUMBER) {
        double value = std::stod(currentToken.text);
        nextToken();
        if (currentToken.type == TokenType::ASSIGNMENT && currentToken.text == "=") {
            std::cout << "Runtime error: invalid assignee." << std::endl;
            exit(3);
            //throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
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
        nextToken();
        if (currentToken.type == TokenType::LEFT_SQUARE) {
            return infixparseArrayLookup(varName);
        }
        return std::make_shared<Variable>(varName);
    } else if (currentToken.type == TokenType::LEFT_PAREN) {
        nextToken();
        std::shared_ptr<ASTNode> result(infixparseExpression());
        if (currentToken.type == TokenType::RIGHT_PAREN) {
            nextToken();
            
        }/* else {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }*/
        return result;
    } else if (currentToken.type == TokenType::RIGHT_PAREN) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    } else {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
}

// START OF ARRAY UTILITY FUNCTIONS
// Implement the new array utility functions
double infixParser::getArrayLength(std::shared_ptr<ASTNode> array) {
    auto arrayLiteral = std::dynamic_pointer_cast<ArrayLiteral>(array);
    if (arrayLiteral) {
        return static_cast<double>(arrayLiteral->elements.size());
    } else {
        throw std::runtime_error("Runtime error: not an array");
    }
}

double infixParser::arrayPop(std::shared_ptr<ASTNode> array) {
    auto arrayLiteral = std::dynamic_pointer_cast<ArrayLiteral>(array);
    if (arrayLiteral && !arrayLiteral->elements.empty()) {
        // Assuming the array contains numeric values
        auto poppedValue = std::dynamic_pointer_cast<Number>(arrayLiteral->elements.back());
        
        if (poppedValue) {
            // Successfully cast to Number
            arrayLiteral->elements.pop_back();
            return poppedValue->value;
        } else {
            throw std::runtime_error("Runtime error: array element is not a numeric value");
        }
    } else {
        throw std::runtime_error("Runtime error: underflow");
    }
}


void infixParser::arrayPush(std::shared_ptr<ASTNode> array, double value) {
    auto arrayLiteral = std::dynamic_pointer_cast<ArrayLiteral>(array);
    if (arrayLiteral) {
        // Create a shared_ptr<Number> from the double value
        auto numberValue = std::make_shared<Number>(value);
        arrayLiteral->elements.push_back(numberValue);
    } else {
        throw std::runtime_error("Runtime error: not an array");
    }
}

// END OF ARRAY UTILITY FUNCTIONS

// START OF ARRAY PARSING FUNCTIONS
// Implement the new array parsing functions
std::shared_ptr<ArrayLiteral> infixParser::infixparseArrayLiteral() {
    // Parse array literals here
    auto arrayLiteral = std::make_shared<ArrayLiteral>();
    nextToken(); // Consume '['
    
    while (currentToken.type != TokenType::RIGHT_SQUARE) {
        auto expression = infixparseExpression();
        arrayLiteral->elements.push_back(expression);
        if (currentToken.type == TokenType::COMMA) {
            nextToken(); // Consume ','
        } else if (currentToken.type != TokenType::RIGHT_SQUARE) {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
    }

    nextToken(); // Consume ']'
    return arrayLiteral;
}


std::shared_ptr<ArrayLookup> infixParser::infixparseArrayLookup(std::string arrayName) {
    nextToken(); // Consume '['
    auto indexExpression = infixparseExpression();
    if (currentToken.type == TokenType::RIGHT_SQUARE) {
        nextToken(); // Consume ']'
        auto arrayLookup = std::make_shared<ArrayLookup>(arrayName, indexExpression);
        /*if (currentToken.type == TokenType::SEMICOLON) {
            nextToken(); // Consume ';'
        }*/
        return std::make_shared<ArrayLookup>(arrayName, indexExpression);
    } else {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
}

// END OF ARRAY PARSING FUNCTIONS



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
    } else if (std::dynamic_pointer_cast<ArrayLiteral>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayLiteral>(node);
        return statement->toInfix();
    } else if (std::dynamic_pointer_cast<ArrayLookup>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayLookup>(node);
        return statement->toInfix();
    } else if (std::dynamic_pointer_cast<ArrayAssignment>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayAssignment>(node);
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
    } else if (std::dynamic_pointer_cast<ArrayLiteral>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayLiteral>(node);
        return statement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ArrayLookup>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayLookup>(node);
        return statement->evaluate(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ArrayAssignment>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayAssignment>(node);
        return statement->evaluate(mainSymbolTable);
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}


std::vector<std::string> Assignment::evaluateToArray(std::map<std::string, double>& symbolTable) {
    auto result = expression->evaluateToArray(symbolTable);
    if (arrayTable.find(variableName) != arrayTable.end()) {
        return {};
    }
    if (!result.empty()) {
        symbolTable[variableName] = std::stod(result[0]);
    }
    return {};
}

std::vector<std::string> Variable::evaluateToArray(std::map<std::string, double>& symbolTable) {
    if (variableName == "null") return {};
    if (arrayTable.find(variableName) != arrayTable.end()) {
        auto arrayLiteral = arrayTable[variableName];
        return arrayLiteral->values;
    }
    if (symbolTable.find(variableName) != symbolTable.end()) {
        auto result = symbolTable.at(variableName);
        auto ret_str = std::to_string(result);
        return {{ret_str}};
    } else {
        throw UnknownIdentifierException(symbolTable, variableName);
    }
}

std::vector<std::string> BinaryOperation::evaluateToArray(std::map<std::string, double>& symbolTable) {
    double leftValue = 0.0;
    double rightValue = 0.0;
    auto leftValues = left->evaluateToArray(symbolTable);
    auto rightValues = right->evaluateToArray(symbolTable);
    if (!leftValues.empty()) leftValue = std::stod((leftValues)[0]);
    if (!rightValues.empty()) rightValue = std::stod((rightValues)[0]);

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
    double value = 0.0;
    bool getValue = true;
    if (op == "+") {
        value = leftValue + rightValue;
    } else if (op == "-") {
        value = leftValue - rightValue;
    } else if (op == "*") {
        value = leftValue * rightValue;
    } else if (op == "/") {
        if (rightValue == 0) {
            throw DivisionByZeroException();
        }
        value = leftValue / rightValue;
    } else if (op == "%") {
        value = std::fmod(leftValue, rightValue);
    } else {
        getValue = false;
    }
    if (getValue) {
        return {{std::to_string(value)}};
    }
    std::string value_str;
    getValue = true;
    if (op == "<") value_str = leftValue < rightValue ? 1 : 0;
    else if (op == ">") value_str = leftValue > rightValue ? 1 : 0;
    else if (op == "<=") value_str = leftValue <= rightValue ? 1 : 0;
    else if (op == ">=") value_str = leftValue >= rightValue ? 1 : 0;
    else if (op == "==") value_str = leftValue == rightValue ? 1 : 0;
    else if (op == "!=") value_str = leftValue != rightValue ? 1 : 0;
    else if (op == "&") value_str = static_cast<int>(leftValue) & static_cast<int>(rightValue);
    else if (op == "^") value_str = static_cast<int>(leftValue) ^ static_cast<int>(rightValue);
    else if (op == "|") value_str = static_cast<int>(leftValue) | static_cast<int>(rightValue);
    else getValue = false;
    if (getValue) {
        return {{value_str}};
    }  
    throw InvalidOperatorException();
}

std::vector<std::string> BooleanNode::evaluateToArray(std::map<std::string, double>& /* unused */) {
    if (value) return {{"true"}};
    return {{"false"}};  
}

std::vector<std::string> Number::evaluateToArray(std::map<std::string, double>& /* unused */) {
    auto result = std::to_string(value);
    return {{result}};
}

std::vector<std::string> Block::evaluateToArray(std::map<std::string, double>& symbolTable) {
    std::vector<std::string> result;
    for (auto statement : statements) {
        result = statement->evaluateToArray(symbolTable);
    }
    return result;
}

std::vector<std::string> BracedBlock::evaluateToArray(std::map<std::string, double>& symbolTable) {
    std::vector<std::string> result;
    if (block) {
        result = block->evaluateToArray(symbolTable);
    }
    return result;
}

std::vector<std::string> IfStatement::evaluateToArray(std::map<std::string, double>& symbolTable) {
    std::vector<std::string> result;
    if (condition && !condition->evaluateToArray(symbolTable).empty()) {
        if (bracedBlock) result = bracedBlock->evaluateToArray(symbolTable);
    } else if (elseNode) result = elseNode->evaluateToArray(symbolTable);
    return result;
}

std::vector<std::string> ElseStatement::evaluateToArray(std::map<std::string, double>& symbolTable) {
    std::vector<std::string> result;
    if (ifStatement) {
        result = ifStatement->evaluateToArray(symbolTable);
    } else if (bracedBlock) {
        result = bracedBlock->evaluateToArray(symbolTable);
    }
    return result;
}

std::vector<std::string> WhileStatement::evaluateToArray(std::map<std::string, double>& symbolTable) {
    std::vector<std::string> result;
    while (condition && !condition->evaluateToArray(symbolTable).empty()) {
        if (bracedBlock) result = bracedBlock->evaluateToArray(symbolTable);
    }
    return result;
}

std::vector<std::string> PrintStatement::evaluateToArray(std::map<std::string, double>& symbolTable) {
    auto results = expression->evaluateToArray(symbolTable);
    return results;
}

std::vector<std::string> FunctionDefinition::evaluateToArray(std::map<std::string, double>& symbolTable) {
    if (!isCalled) {
        for (const auto& [key, value] : symbolTable) {
            mySymbolTable[key] = value;
        }
        return {};
    }
    auto result = bracedBlock->evaluateToArray(mySymbolTable);
    return result;
}

std::vector<std::string> FunctionReturn::evaluateToArray(std::map<std::string, double>& symbolTable) {
    auto result = expression->evaluateToArray(symbolTable);
    return result;   
}

std::vector<std::string> FunctionCall::evaluateToArray(std::map<std::string, double>& symbolTable) {
    if (isAliasName) return {};
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
        auto result = parameter.second->evaluateToArray(calledFunction->mySymbolTable);
        if (!result.empty()) {
            calledFunction->mySymbolTable[parameter.first] = std::stod(result[0]);
        }
    }
    auto result = calledFunction->evaluateToArray(symbolTable);
    return result;
}

std::vector<std::string> ArrayLiteral::evaluateToArray(std::map<std::string, double>& symbolTable) {
    for (const auto& element : elements) {
        auto results = element->evaluateToArray(symbolTable);
        if (!results.empty()) {
            values.push_back(results[0]);
          
        }
    }
    for (const auto& value : values) {
    }
    return values;
}

std::vector<std::string> ArrayLookup::evaluateToArray(std::map<std::string, double>& symbolTable) {
    // Check if the array and index are valid
    if (arrayTable.find(arrayName) == arrayTable.end()) {
        throw std::runtime_error("Invalid array type for lookup.");
    }
    if (!index) {
        throw std::runtime_error("index cannot be null.");
    }
    auto arrayLiteral = arrayTable[arrayName];
    // Evaluate index to get their values
    auto indexValue = index->evaluate(symbolTable);
    // Check if the index is a valid integer
    if (std::floor(indexValue) != indexValue) {
        throw std::runtime_error("Array index must be an integer.");
    }
    int arrayIndex = static_cast<int>(indexValue);
    // Check if the array index is within bounds
    if (arrayIndex < 0 || arrayIndex >= static_cast<int>(arrayLiteral->elements.size())) {
        throw std::runtime_error("Array index out of bounds.");
    }
    // Use the ArrayLiteral class method to get the value at the specified index
    return arrayLiteral->elements[arrayIndex]->evaluateToArray(symbolTable);
}

std::vector<std::string> ArrayAssignment::evaluateToArray(std::map<std::string, double>& symbolTable) {
    // Check if the array and index are valid
    if (arrayTable.find(arrayName) == arrayTable.end()) {
        throw std::runtime_error("Invalid array type for assignment.");
    }
    if (!index) {
        throw std::runtime_error("index cannot be null.");
    }
    auto arrayLiteral = arrayTable[arrayName];
    // Evaluate index to get their values
    auto indexValue = index->evaluate(symbolTable);
    // Check if the index is a valid integer
    if (std::floor(indexValue) != indexValue) {
        throw std::runtime_error("Array index must be an integer.");
    }
    int arrayIndex = static_cast<int>(indexValue);
    // Check if the array index is within bounds
    if (arrayIndex < 0 || arrayIndex >= static_cast<int>(arrayLiteral->elements.size())) {
        throw std::runtime_error("Array index out of bounds.");
    }
    auto result = asignment->evaluateToArray(symbolTable);
    arrayLiteral->elements[arrayIndex] = asignment;
    arrayLiteral->values[arrayIndex] = result[0];
    return {};
}


std::vector<std::string> infixParser::evaluateToArray(std::shared_ptr<ASTNode> node) {
    if (std::dynamic_pointer_cast<BinaryOperation>(node) != nullptr) {
        auto obj = std::dynamic_pointer_cast<BinaryOperation>(node);
        return obj->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<BooleanNode>(node) != nullptr) {
        auto obj = std::dynamic_pointer_cast<BooleanNode>(node);
        return obj->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Number>(node) != nullptr) {
        auto obj = std::dynamic_pointer_cast<Number>(node);
        return obj->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Assignment>(node) != nullptr) {
        auto assignment = std::dynamic_pointer_cast<Assignment>(node);
        return assignment->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Variable>(node) != nullptr) {
        auto variable = std::dynamic_pointer_cast<Variable>(node);
        return variable->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<Block>(node) != nullptr) {
        auto block = std::dynamic_pointer_cast<Block>(node);
        return block->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<BracedBlock>(node) != nullptr) {
        auto block = std::dynamic_pointer_cast<BracedBlock>(node);
        return block->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<IfStatement>(node) != nullptr) {
        auto ifStatement = std::dynamic_pointer_cast<IfStatement>(node);
        return ifStatement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ElseStatement>(node) != nullptr) {
        auto elseStatement = std::dynamic_pointer_cast<ElseStatement>(node);
        return elseStatement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<WhileStatement>(node) != nullptr) {
        auto whileStatement = std::dynamic_pointer_cast<WhileStatement>(node);
        return whileStatement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<PrintStatement>(node) != nullptr) {
        auto printStatement = std::dynamic_pointer_cast<PrintStatement>(node);
        return printStatement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<FunctionDefinition>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionDefinition>(node);
        return statement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<FunctionReturn>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionReturn>(node);
        return statement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<FunctionCall>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<FunctionCall>(node);
        return statement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ArrayLiteral>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayLiteral>(node);
        return statement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ArrayLookup>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayLookup>(node);
        return statement->evaluateToArray(mainSymbolTable);
    } else if (std::dynamic_pointer_cast<ArrayAssignment>(node) != nullptr) {
        auto statement = std::dynamic_pointer_cast<ArrayAssignment>(node);
        return statement->evaluateToArray(mainSymbolTable);
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}
