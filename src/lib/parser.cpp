#include "parser.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>

std::unordered_map<std::string, double> Node::variableMap;

Node::~Node() {
    for (Node* child : children) {
        delete child;
    }
}

//Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0), roots({}) {}
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), currentTokenIndex(0) {}

Parser::~Parser() {
    if (!roots.empty()) {
        for (auto root : roots) {
            delete root;
        }
    }
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
    //std::cout << "test parseExpression(): (" << currentTokenIndex << " : " << tokens[currentTokenIndex].line << " : " << tokens[currentTokenIndex].column<< ") : " << (int)tokens[currentTokenIndex].type << ", " << tokens[currentTokenIndex].text << std::endl;
        if (tokens[currentTokenIndex].type == TokenType::LEFT_PAREN) {
            currentTokenIndex++;
            std::string next_token = tokens[currentTokenIndex].text;
            
            
            if (next_token != "+" && next_token != "-" && next_token != "*" && next_token != "/"&& next_token != "=") {
                std::cout << "Unexpected token at line " << tokens[currentTokenIndex].line
                << " column " << tokens[currentTokenIndex].column
                << ": " << tokens[currentTokenIndex].text << std::endl;
                exit(2);
            }
            node->type = tokens[currentTokenIndex].type;
            node->value = tokens[currentTokenIndex++].text;
            
            /*if (next_token == "=") {
              if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
            }*/
            
            while (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type != TokenType::RIGHT_PAREN) {
                node->children.push_back(parseExpression()); 
            }
            if (currentTokenIndex < tokens.size() && tokens[currentTokenIndex].type == TokenType::RIGHT_PAREN) {
                currentTokenIndex++;
                
                
                
    //std::cout << "test parseExpression(): 2 (" << currentTokenIndex << " : " << tokens[currentTokenIndex].line << " : " << tokens[currentTokenIndex].column<< ") : " << (int)tokens[currentTokenIndex].type << ", " << tokens[currentTokenIndex].text << std::endl;
                return node;
            } else {
                std::cout << "Unexpected token at line " +
                    std::to_string(tokens[currentTokenIndex].line) + " column " +
                    std::to_string(tokens[currentTokenIndex].column) + ": " +
                    tokens[currentTokenIndex].text << std::endl;
                exit(2);
            }
        } else if (tokens[currentTokenIndex].type == TokenType::NUMBER) {
            node->type = tokens[currentTokenIndex].type;
            node->value = tokens[currentTokenIndex++].text;
    //std::cout << "test parseExpression(): 3(" << currentTokenIndex << " : " << tokens[currentTokenIndex].line << " : " << tokens[currentTokenIndex].column<< ") : " << (int)tokens[currentTokenIndex].type << ", " << tokens[currentTokenIndex].text << std::endl;
            return node;
        } else if (tokens[currentTokenIndex].type == TokenType::ASSIGNMENT) {
            node->type = tokens[currentTokenIndex].type;
            node->value = tokens[currentTokenIndex++].text; 
    //std::cout << "test parseExpression(): 4(" << currentTokenIndex << " : " << tokens[currentTokenIndex].line << " : " << tokens[currentTokenIndex].column<< ") : " << (int)tokens[currentTokenIndex].type << ", " << tokens[currentTokenIndex].text << std::endl;
            return node;
        } else if (tokens[currentTokenIndex].type == TokenType::IDENTIFIER) {
            node->type = tokens[currentTokenIndex].type;
            node->value = tokens[currentTokenIndex++].text;
    //std::cout << "test parseExpression(): 5(" << currentTokenIndex << " : " << tokens[currentTokenIndex].line << " : " << tokens[currentTokenIndex].column<< ") : " << (int)tokens[currentTokenIndex].type << ", " << tokens[currentTokenIndex].text << std::endl;
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
    //std::cout << "test parseExpression(): 6 (" << currentTokenIndex << " : " << tokens[currentTokenIndex].line << " : " << tokens[currentTokenIndex].column << ") : " << (int)tokens[currentTokenIndex].type << ", " << tokens[currentTokenIndex].text << std::endl;
        return nullptr;
    } else {
        std::cout << "Invalid input: Unexpected end of input." << std::endl;
        exit(2);
    }
}

    std::string Parser::printInfix(Node* node) {
    //std::cout << "test printInfix()  type: " << (int)node->type << " value: " << node->value << std::endl;

    if (node == nullptr) {
        return "";  
    } else if (node->type == TokenType::IDENTIFIER) {
      return node->value;
    } else if (node->type == TokenType::NUMBER) {
        double value = std::stod(node->value);
        if (value == std::floor(value)) {
            return std::to_string(static_cast<int>(value));
        }
        else {
            return node->value;
        }
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
    //std::cout << "test  evaluate() (type : value) = " << (int)type << " : " << value << " map " << variableMap[value] << std::endl;


  
    double result = 0.0;

    switch (type) {
    case TokenType::OPERATOR:
        if (value == "+") {
            for (Node* child : children) {
                result += child->evaluate();
            }
        } else if (value == "-") {
            if (children.size() == 0) {
                std::cout <<("Invalid number of children for operator: " + value) << std::endl;
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
                std::cout <<("Invalid number of children for operator: " + value) << std::endl;
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
            std::cout <<("Invalid operator: " + value) << std::endl;
            exit(2);
        }
        break;
    case TokenType::IDENTIFIER:
    //std::cout << "test  evaluate() IDENTIFIER (type : value) = " << (int)type << " : " << value << " map " << variableMap[value] << std::endl;
        result = variableMap[value];
        break;
    case TokenType::ASSIGNMENT:
        if (children.size() == 0) {
            std::cout <<("Invalid number of children for assignment: " + value) << std::endl;
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
    //std::cout << "test evaluate() 2 (type : value) = " << (int)type << " : " << children[i]->value << " map " << variableMap[children[i]->value] << std::endl;
                }
            }
//for (auto& var : Node::variableMap) {
    //std::cout << "test  evaluate() var  = " << var.first << " : " << var.second << std::endl;
//}
        } else {
            std::cout <<("Invalid value for assignment: " + value) << std::endl;
            exit(2);
        }}
        break;
    case TokenType::NUMBER:
        {std::istringstream ss(value);
        ss >> result;
        if (ss.fail()) {
            std::cout <<("Invalid input: " + value) << std::endl;
            exit(2);
        }}
        break; 
    default: 
        {std::cout <<("Invalid input: " + value) << std::endl;
        exit(2);
        break;
      }
    }
    return result;
}

