#ifndef PARSER_H
#define PARSER_H
#include "token.h"
#include <vector>
#include <unordered_map>
#include <memory>  // Include the memory header for smart pointers

class Node {
public:
    static std::unordered_map<std::string, double> variableMap;  // Store variable's value
    std::string value;
    TokenType type;
    std::vector<std::unique_ptr<Node>> children;   // Use std::unique_ptr for children nodes

    Node(const std::string& val) : value(val), type(TokenType::OPERATOR) {}
    Node(const std::string& val, const TokenType& tp) : value(val), type(tp) {}
    ~Node() = default; // Use the default destructor
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
    ~Parser() = default; // Use the default destructor

    std::vector<std::unique_ptr<Node>> parse();  // Use std::unique_ptr for parsed nodes
    std::unique_ptr<Node> parseExpression();  // Use std::unique_ptr for the expression
    double evaluate(const Node* node);
    std::string printInfix(const Node* node);

private:
    const std::vector<Token> tokens;

    size_t currentTokenIndex;
    std::vector<std::unique_ptr<Node>> roots;  // Use std::unique_ptr for parsed roots
};

#endif

// class Node {
// public:
//     static std::unordered_map<std::string, double> variableMap;  //store variable's value
//     std::string value;
//     TokenType type;
//     std::vector<Node*> children;   // vector to store children nodes


//     Node(const std::string& val) : value(val), type(TokenType::OPERATOR) {}
//     Node(const std::string& val, const TokenType& tp) : value(val), type(tp) {}
//     ~Node();
//     double evaluate();

//     int getPrecedence() const {
//         if (value == "*" || value == "/") {
//             return 2;
//         } else if (value == "+" || value == "-") {
//             return 1;
//         } else {
//             return 0;
//         }
//     }
// };

// class Parser {
// public:
//     Parser(const std::vector<Token>& tokens);
//     ~Parser();

//     std::vector<Node*> parse();
//     Node* parseExpression();
//     double evaluate(Node* node);
//     std::string printInfix(Node* node);

// private:
//     const std::vector<Token> tokens;
    
//     size_t currentTokenIndex;
//     std::vector<Node*> roots;
// };

// #endif