#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include <vector>

class Node {
public:
    std::string value;
    std::vector<Node*> children; // vector to store children nodes
    //std::vector<Node*> parse(); //add?
    std::string operation;

    Node(const std::string& val) : value(val), operation("") {}
    Node(const std::string& val, const std::string& op) : value(val), operation(op) {}
    ~Node();
    double evaluate();

    int getPrecedence() const {
        if (operation == "*" || operation == "/") {
            return 2;
        } else if (operation == "+" || operation == "-") {
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

    Node* parse();
    Node* parseExpression();
    double evaluate(Node* node);
    std::string printInfix(Node* node);

private:
    const std::vector<Token> tokens;
    size_t currentTokenIndex;
    Node* root;
};

#endif