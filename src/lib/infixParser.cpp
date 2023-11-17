
#include <sstream>
#include <stdexcept>
#include <memory>
#include <cmath>
#include "infixParser.h"

int indent = 0;
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

double BooleanNode::evaluate(std::map<std::string, double>& /* unused */) const {
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
    if (index < tokens.size() && tokens[index].text != "END") {
       return infixparseStatement();
    }
    return nullptr;
}

ASTNode* infixParser::infixparseStatement() {
    std::string tokenName = currentToken.text;
    if  (tokens.size() == 1 && tokenName == "END") {
        return std::make_unique<EmptyStatement>().release();
    }
    if (tokenName == "if") {
        nextToken();
        std::unique_ptr<IfStatement> satement(infixparseIfStatement());
        return satement.release();
    } else if (tokenName == "while") {
        nextToken();
        std::unique_ptr<ASTNode> condition(infixparseCondition());
        if (!condition) {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        std::unique_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
        if (!bracedBlock) {
            throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
        }
        return std::make_unique<WhileStatement>(condition.release(), bracedBlock.release()).release();
    } else if (tokenName == "print") {
        nextToken();
        std::unique_ptr<ASTNode> expr(infixparseExpression());        
        return std::make_unique<PrintStatement>(expr.release()).release();
    }
    return infixparseAssignment();
}

ASTNode* infixParser::infixparseCondition() {
    std::unique_ptr<ASTNode> left (infixparseFactor());
    while (currentToken.type == TokenType::OPERATOR && currentToken.text != "{") {
        std::string op = currentToken.text;
        nextToken();  
        std::unique_ptr<ASTNode> right(infixparseFactor());
        left = std::make_unique<BinaryOperation>(op, left.release(), right.release());
    }
    return left.release();
}

BracedBlock* infixParser::infixparseBracedBlock() {
    if (currentToken.text != "{") { 
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    nextToken();
    auto statement = infixparseStatement();
    if (statement) {
        auto blk = std::make_unique<Block>(statement).release();
        auto bracedBlock = std::make_unique<BracedBlock>(blk).release();
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

IfStatement* infixParser::infixparseIfStatement() {
    std::unique_ptr<ASTNode> condition(infixparseCondition());
    if (!condition) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    std::unique_ptr<BracedBlock> bracedBlock(infixparseBracedBlock());
    if (!bracedBlock) {
        throw UnexpectedTokenException(currentToken.text, currentToken.line, currentToken.column);
    }
    auto ifStatement = std::make_unique<IfStatement>(condition.release(), bracedBlock.release()).release();
    if (currentToken.text == "else") {
        nextToken();
        std::unique_ptr<ElseStatement> elseNode (infixparseElseStatement());
        ifStatement->elseNode = elseNode.release();
    }
    return ifStatement;
}

ElseStatement* infixParser::infixparseElseStatement() {
    BracedBlock* blk = nullptr;
    IfStatement* ifStatement = nullptr;
    if (currentToken.text == "{") {
        blk = infixparseBracedBlock();
    } else if (currentToken.text == "if") {
        nextToken();
        ifStatement = infixparseIfStatement();
    }
    return std::make_unique<ElseStatement>(ifStatement, blk).release();
}



Block::Block(ASTNode* statement) {
    statements.push_back(statement);
}

Block::~Block() {
    for (auto statement : statements) {
        delete statement;
    }
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

double Block::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    for (auto statement : statements) {
        result = statement->evaluate(symbolTable);
    }
    return result;   
}

BracedBlock::BracedBlock(Block* blk)
    : block(blk) {}

BracedBlock::~BracedBlock() {
    delete block;
}

std::string BracedBlock::toInfix() const {
    std::string ret_str;
    if (block) ret_str += block->toInfix();
    return ret_str;
}

double BracedBlock::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    if (block) {
        result = block->evaluate(symbolTable);
    }
    return result;   
}


IfStatement::IfStatement(ASTNode* cond, BracedBlock* blk)
    : condition(cond), bracedBlock(blk) {}

IfStatement::~IfStatement() {
    if (bracedBlock) delete bracedBlock;
    if (elseNode) delete elseNode;
}

std::string IfStatement::toInfix() const {
    std::string ret_str = "if " + condition->toInfix() + " {";
    if (bracedBlock) ret_str += bracedBlock->toInfix();
    if (elseNode) ret_str += elseNode->toInfix();
    return ret_str;
}

double IfStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    if (condition && condition->evaluate(symbolTable) != 0.0) {
        if (bracedBlock) result = bracedBlock->evaluate(symbolTable);
    } else if (elseNode) result = elseNode->evaluate(symbolTable);
    return result;
}


ElseStatement::ElseStatement(IfStatement* state, BracedBlock* blk)
    : ifStatement(state), bracedBlock(blk) {}

ElseStatement::~ElseStatement() {
    if (ifStatement) delete ifStatement;
    if (bracedBlock) delete bracedBlock;
}

std::string ElseStatement::toInfix() const {
    std::string ret_str = "\n" + std::string(indent, ' ') + "else ";
    if (ifStatement) ret_str += ifStatement->toInfix();
    else if (bracedBlock) ret_str += "{" + bracedBlock->toInfix();
    return ret_str;
}

double ElseStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    if (ifStatement) {
        result = ifStatement->evaluate(symbolTable);
    } else if (bracedBlock) {
        result = bracedBlock->evaluate(symbolTable);
    }
    return result;   
}


WhileStatement::WhileStatement(ASTNode* cond, BracedBlock* blk)
    : condition(cond), bracedBlock(blk) {}

WhileStatement::~WhileStatement() {
    if (condition) delete condition;
    if (bracedBlock) delete bracedBlock;
}

std::string WhileStatement::toInfix() const {
    std::string ret_str = "while " + condition->toInfix() + " {";
    if (bracedBlock) ret_str += bracedBlock->toInfix();
    return ret_str;
}

double WhileStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = 0.0;
    while (condition && condition->evaluate(symbolTable) != 0.0) {
        if (bracedBlock) result = bracedBlock->evaluate(symbolTable);
    }
    return result;
}


PrintStatement::PrintStatement(ASTNode* expression)
    : expression(expression) {}

PrintStatement::~PrintStatement() {
    if (expression) delete expression;
}

std::string PrintStatement::toInfix() const {
    return "print " + expression->toInfix();
}

double PrintStatement::evaluate(std::map<std::string, double>& symbolTable) const {
    double result = expression->evaluate(symbolTable);
    return result;   
}


double EndStatement::evaluate(std::map<std::string, double>& /* unused */) const {
    return 0.0;  
}

double EmptyStatement::evaluate(std::map<std::string, double>& /* unused */) const {
    return 0.0;  
}

ASTNode* infixParser::infixparseExpression() {
    return infixparseAssignment();
}

BinaryOperation::~BinaryOperation() {
    if (left) delete left;
    if (right) delete right;
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
    } else if (dynamic_cast<BooleanNode*>(node) != nullptr) {
        return dynamic_cast<BooleanNode*>(node)->toInfix();
    } else if (dynamic_cast<Variable*>(node) != nullptr) {
        Variable* variable = dynamic_cast<Variable*>(node);
        return variable->variableName;
    } else if (dynamic_cast<Block*>(node) != nullptr) {
        Block* block = dynamic_cast<Block*>(node);
        return block->toInfix();
    } else if (dynamic_cast<BracedBlock*>(node) != nullptr) {
        BracedBlock* block = dynamic_cast<BracedBlock*>(node);
        std::string block_str = block->toInfix();
        return block_str;
    } else if (dynamic_cast<IfStatement*>(node) != nullptr) {
        IfStatement* ifStatement = dynamic_cast<IfStatement*>(node);
        return ifStatement->toInfix();
    } else if (dynamic_cast<ElseStatement*>(node) != nullptr) {
        ElseStatement* elseStatement = dynamic_cast<ElseStatement*>(node);
        return elseStatement->toInfix();
    } else if (dynamic_cast<WhileStatement*>(node) != nullptr) {
        WhileStatement* whileStatement = dynamic_cast<WhileStatement*>(node);
        std::string while_str = whileStatement->toInfix();
        return while_str;
    } else if (dynamic_cast<PrintStatement*>(node) != nullptr) {
        PrintStatement* printStatement = dynamic_cast<PrintStatement*>(node);
        return "print " + printInfix(printStatement->expression);
    } else if (dynamic_cast<EndStatement*>(node) != nullptr) {
        return "}";
    } else if (dynamic_cast<EmptyStatement*>(node) != nullptr) {
        return "";
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}


double infixParser::evaluate(ASTNode* node, std::map<std::string, double>& symbolTable) {
    if (dynamic_cast<BinaryOperation*>(node) != nullptr) {
        BinaryOperation* obj = dynamic_cast<BinaryOperation*>(node);
        return obj->evaluate(symbolTable);
    } else if (dynamic_cast<Number*>(node) != nullptr) {
        Number* obj = dynamic_cast<Number*>(node);
        return obj->evaluate(symbolTable);
    } else if (dynamic_cast<Assignment*>(node) != nullptr) {
        Assignment* assignment = dynamic_cast<Assignment*>(node);
        return assignment->evaluate(symbolTable);
    } else if (dynamic_cast<Variable*>(node) != nullptr) {
        Variable* variable = dynamic_cast<Variable*>(node);
        return variable->evaluate(symbolTable);
    } else if (dynamic_cast<Block*>(node) != nullptr) {
        Block* block = dynamic_cast<Block*>(node);
        return block->evaluate(symbolTable);
    } else if (dynamic_cast<BracedBlock*>(node) != nullptr) {
        BracedBlock* block = dynamic_cast<BracedBlock*>(node);
        return block->evaluate(symbolTable);
    } else if (dynamic_cast<IfStatement*>(node) != nullptr) {
        IfStatement* ifStatement = dynamic_cast<IfStatement*>(node);
        return ifStatement->evaluate(symbolTable);
    } else if (dynamic_cast<ElseStatement*>(node) != nullptr) {
        ElseStatement* elseStatement = dynamic_cast<ElseStatement*>(node);
        return elseStatement->evaluate(symbolTable);
    } else if (dynamic_cast<WhileStatement*>(node) != nullptr) {
        WhileStatement* whileStatement = dynamic_cast<WhileStatement*>(node);
        return whileStatement->evaluate(symbolTable);
    } else if (dynamic_cast<PrintStatement*>(node) != nullptr) {
        PrintStatement* printStatement = dynamic_cast<PrintStatement*>(node);
        return printStatement->evaluate(symbolTable);
    } else {
        std::cout << "Invalid node type" << std::endl;
        exit(4);
    }
}