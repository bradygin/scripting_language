#include "parser.h"
#include <iostream>
#include <sstream>
#include <cmath>

std::unordered_map<std::string, double> Node::variableMap;

Node::~Node() {
    for (Node* child : children) {
        delete child;
    }
}

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0) {}

Parser::~Parser() {
}

std::vector<Node*> Parser::parse() {
    while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].text != "END") {
        auto root = parseExpression();
        roots.push_back(root);
    }
    return roots;
}

Node* Parser::parseExpression() {
    Node* node = new Node("");
    while (currentTokenIndex < tokens.size()) {
        if (tokens[currentTokenIndex].type == TokenType::LEFT_PAREN) {
            currentTokenIndex++;
            std::string next_token = tokens[currentTokenIndex].text;

            if (next_token != "+" && next_token != "-" && next_token != "*" && next_token != "/" && next_token != "=") {
                if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PAREN) {
                    std::cout << "1 Unexpected token at line " << tokens[currentTokenIndex].line
                              << " column " << tokens[currentTokenIndex].column
                              << ": " << tokens[currentTokenIndex].text << std::endl;
                } else {
                    std::cout << "2 Unexpected token at line " << tokens[currentTokenIndex].line
                              << " column " << tokens[currentTokenIndex].column
                              << ": " << tokens[currentTokenIndex].text << std::endl;
                }
                exit(2);
            }
            node->type = tokens[currentTokenIndex].type;
            node->value = tokens[currentTokenIndex++].text;

            while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::RIGHT_PAREN) {
                node->children.push_back(parseExpression());
            }
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PAREN) {
                currentTokenIndex++;
                if (node->type == TokenType::ASSIGNMENT) {
                    // Check for unexpected token cases in assignment
                    if (node->children.empty()) {
                        std::cout << "3 Unexpected token at line " << tokens[currentTokenIndex].line
                                  << " column " << tokens[currentTokenIndex].column
                                  << ": " << tokens[currentTokenIndex].text << std::endl;
                        exit(2);
                    } else if (node->children.size() == 1) {
                        std::cout << "4 Unexpected token at line " << tokens[currentTokenIndex].line
                                  << " column " << tokens[currentTokenIndex].column
                                  << ": " << tokens[currentTokenIndex].text << std::endl;
                        exit(2);
                    }
                }
                return node;
            } else {
                if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PAREN) {
                    std::cout << "5 Unexpected token at line " << tokens[currentTokenIndex].line
                              << " column " << tokens[currentTokenIndex].column
                              << ": " << tokens[currentTokenIndex].text << std::endl;
                } else {
                    std::cout << "6 Unexpected token at line " << tokens[currentTokenIndex].line
                              << " column " << tokens[currentTokenIndex].column
                              << ": " << tokens[currentTokenIndex].text << std::endl;
                }
                exit(2);
            }
        } else if (tokens[currentTokenIndex].type == TokenType::NUMBER || tokens[currentTokenIndex].type == TokenType::IDENTIFIER || tokens[currentTokenIndex].type == TokenType::ASSIGNMENT) {
            node->type = tokens[currentTokenIndex].type;
            node->value = tokens[currentTokenIndex++].text;
            return node;
        } else {
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PAREN) {
               std::cout << "7 Unexpected token at line " << tokens[currentTokenIndex].line
                          << " column " << tokens[currentTokenIndex].column
                          << ": " << tokens[currentTokenIndex].text << std::endl;
            } else {
                std::cout << "8 Unexpected token at line " << tokens[currentTokenIndex].line
                          << " column " << tokens[currentTokenIndex].column
                          << ": " << tokens[currentTokenIndex].text << std::endl;
            }
            exit(2);
        }
    }
    std::cout << "Invalid input: Unexpected end of input." << std::endl;
    exit(2);
}




std::string customToString(double numericValue) {
    std::string stringValue = std::to_string(numericValue);

    size_t pos = stringValue.find_last_not_of('0');
    if (pos != std::string::npos && stringValue[pos] == '.') {
        pos--; // Remove the trailing decimal point
    }

    return stringValue.substr(0, pos + 1);
}

std::string Parser::printInfix(Node* node) {
    if (node == nullptr) {
        return "";
    } else if (node->type == TokenType::IDENTIFIER) {
        return node->value;
    } else if (node->type == TokenType::NUMBER) {
        double numericValue = std::stod(node->value);
        std::string castedValue = customToString(numericValue);
        return castedValue;
    } else {
        std::string infix = "(";
        for (size_t i = 0; i < node->children.size(); ++i) {
            infix += printInfix(node->children[i]);
            if (i < node->children.size() - 1) {
                infix += " " + node->value + " ";
            }
        }
        infix += ")";
        return infix;
    }
}




double Node::evaluate() {
    double result = 0.0;

    if (type == TokenType::OPERATOR) {
        if (value == "+") {
            for (Node* child : children) {
                result += child->evaluate();
            }
        } else if (value == "-") {
            if (children.size() == 0) {
                std::cout << "Invalid number of children for operator: " + value << std::endl;
                exit(2);
            }
            result = children[0]->evaluate();
            for (size_t i = 1; i < children.size(); ++i) {
                result -= children[i]->evaluate();
            }
        } else if (value == "*") {
            result = 1.0;
            for (Node* child : children) {
                result *= child->evaluate();
            }
        } else if (value == "/") {
            if (children.size() == 0) {
                std::cout << "Invalid number of children for operator: " + value << std::endl;
                exit(3);
            }
            result = children[0]->evaluate();
            for (size_t i = 1; i < children.size(); ++i) {
                if (children[i]->evaluate() == 0.0) {
                    std::cout << "Runtime error: division by zero." << std::endl;
                    exit(3);
                }
                result /= children[i]->evaluate();
            }
        } else {
            std::cout << "Invalid operator: " + value << std::endl;
            exit(2);
        }
    } else if (type == TokenType::IDENTIFIER) {
        result = variableMap[value];
    } else if (type == TokenType::ASSIGNMENT) {
        if (children.size() == 0) {
            std::cout << "Invalid number of children for assignment: " + value << std::endl;
            exit(2);
        } else {
            bool found_result = false;
            for (size_t i = 0; i < children.size(); ++i) {
                if (children[i]->type != TokenType::IDENTIFIER) {
                    found_result = true;
                    result = children[i]->evaluate();
                }
            }
            if (found_result) {
                for (size_t i = 0; i < children.size(); ++i) {
                    if (children[i]->type == TokenType::IDENTIFIER) {
                        variableMap[children[i]->value] = result;
                    }
                }
            } else {
                std::cout << "Invalid value for assignment: " + value << std::endl;
                exit(2);
            }
        }
    } else if (type == TokenType::NUMBER) {
        std::istringstream ss(value);
        ss >> result;
        if (ss.fail()) {
            std::cout << "Invalid input: " + value << std::endl;
            exit(2);
        }
    } else {
        std::cout << "Invalid input: " + value << std::endl;
        exit(2);
    }

    return result;
}