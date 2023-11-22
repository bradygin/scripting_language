#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include <memory>
#include <vector>
#include <unordered_map>

class Node {
public:
    static std::unordered_map<std::string, double> variableMap;  //store variable's value
    std::string value;
    TokenType type;
    std::vector<std::shared_ptr<Node>> children;   // Vector to store children nodes


    Node(const std::string& val) : value(val), type(TokenType::OPERATOR) {}
    Node(const std::string& val, const TokenType& tp) : value(val), type(tp) {}
    ~Node() = default;
    double evaluate();

    int getPrecedence() const {
        if (value == "*" || value == "/") {
            return 2;
        } else if (value == "+" || value == "-") {
            return 1;
        } else {
            return 0;
        }
    }
};

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    ~Parser() = default;
    std::vector<std::shared_ptr<Node>> parse();
    std::shared_ptr<Node> parseExpression();
    double evaluate(std::shared_ptr<Node> node);
    std::string printInfix(std::shared_ptr<Node> node);

private:
    const std::vector<Token> tokens;
    
    size_t currentTokenIndex;
    std::vector<std::shared_ptr<Node>> roots;
};

#endif