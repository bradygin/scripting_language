#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

Node::~Node() {
    for (Node* child : children) {
        delete child;
    }
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0), root(nullptr) {}

Parser::~Parser() {
    if (root) {
        delete root;
    }
}

Node* Parser::parse() {
    root = parseExpression();
    if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].text != "END") {
        std::cout <<("Unexpected token at line " + std::to_string(tokens[currentTokenIndex].line) + " column " 
                    + std::to_string(tokens[currentTokenIndex].column) + ": " + tokens[currentTokenIndex].text) << std::endl;
        exit(2);
    }
    return root;
}

Node* Parser::parseExpression() {
    Node* node = new Node(""); 
    while (currentTokenIndex < tokens.size()) {
        if (tokens[currentTokenIndex].type == TokenType::LEFT_PAREN) {
            currentTokenIndex++;
            node->operation = tokens[currentTokenIndex].text; 
            currentTokenIndex++;
            while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::RIGHT_PAREN) {
                node->children.push_back(parseExpression()); 
            }
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PAREN) {
                currentTokenIndex++;
                return node;
            } else {
                std::cout << "Unexpected token at line " +
                    std::to_string(tokens[currentTokenIndex].line) + " column " +
                    std::to_string(tokens[currentTokenIndex].column) + ": " +
                    tokens[currentTokenIndex].text << std::endl;
                exit(2);
            }
        } else if (tokens[currentTokenIndex].type == TokenType::NUMBER) {
            node->value = tokens[currentTokenIndex++].text;
            return node;
        } else {
            std::cout << "Unexpected token at line " +
                std::to_string(tokens[currentTokenIndex].line) + " column " +
                std::to_string(tokens[currentTokenIndex].column) + ": " +
                tokens[currentTokenIndex].text << std::endl;
            exit(2);
        }
    }
    if (tokens[currentTokenIndex].text == "END") {
        return nullptr;
    } else {
        std::cout << "Invalid input: Unexpected end of input." << std::endl;
        exit(2);
    }
}

std::string Parser::printInfix(Node* node) {
    if (node == nullptr) {
        return "";  
    } else if (node->operation == "") {
        return node->value;
    } else {
        std::string infix = "(";
        for (size_t i = 0; i < node->children.size(); ++i) {
            infix += printInfix(node->children[i]);
            if (i < node->children.size() - 1) {
                infix += " " + node->operation + " ";
            }
        }
        infix += ")";
        return infix;
    }
}

double Node::evaluate() {
    if (operation == "") {
        std::istringstream ss(value);
        double result;
        ss >> result;
        if (ss.fail()) {
            std::cout <<("Invalid input: " + value) << std::endl;
            exit(2);
        }
        return result;
    }

    double result = 0.0;

    if (operation == "+") {
        for (Node* child : children) {
            result += child->evaluate();
        }
    } else if (operation == "-") {
        if (children.size() == 0) {
            std::cout <<("Invalid number of children for operator: " + operation) << std::endl;
            exit(2);
        }
        result = children[0]->evaluate();
        for (size_t i = 1; i < children.size(); ++i) {
            result -= children[i]->evaluate();
        }
    } else if (operation == "*") {
        result = 1.0;
        for (Node* child : children) {
            result *= child->evaluate();
        }
    } else if (operation == "/") {
        if (children.size() == 0) {
            std::cout <<("Invalid number of children for operator: " + operation) << std::endl;
            exit(3);
        }
        result = children[0]->evaluate();
        for (size_t i = 1; i < children.size(); ++i) {
            if (children[i]->evaluate() == 0.0) {
                std::cout <<("Runtime error: division by zero.") << std::endl;
                exit(3);
            }
            result /= children[i]->evaluate();
        }
    } else {
        std::cout <<("Invalid operator: " + operation) << std::endl;
        exit(2);
    }

    return result;
}

