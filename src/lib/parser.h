#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include <vector>
#include <unordered_map>

class Node {
public:
    static std::unordered_map<std::string, double> variableMap;  //store variable's value
    std::string value;
    TokenType type;
    std::vector<Node*> children;   // vector to store children nodes


    Node(const std::string& val) : value(val), type(TokenType::OPERATOR) {}
    Node(const std::string& val, const TokenType& tp) : value(val), type(tp) {}
    ~Node();
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
    ~Parser();

    std::vector<Node*> parse();
    Node* parseExpression();
    double evaluate(Node* node);
    std::string printInfix(Node* node);

private:
    const std::vector<Token> tokens;
    
    size_t currentTokenIndex;
    std::vector<Node*> roots;
};

#endif