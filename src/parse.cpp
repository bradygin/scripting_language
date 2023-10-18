#include <iostream>
#include <fstream>
#include <iomanip>

#include "lexer.h"  
#include "token.h"  
#include "parser.h" 
/*
g++ -o my_parser parser.cpp lexer.cpp testParser.cpp parser.h
./my_parser < input.txt
*/

int main() {
    // Open a file containing your input expression
    std::cout << "Enter your input:" << std::endl;

    // Initialize the lexer with standard input (cin)
    Lexer lexer(std::cin);

    try {
        /* Create a vector of tokens by calling the lexer's tokenize() member function which uses 
        the nextToken() helper function to create tokens and then pushes them into a vector */
        //std::cout << "Before tokenize()" << std::endl << std::flush;

        std::vector<Token> tokens = lexer.tokenize();

        //std::cout << "After tokenize()" << std::endl << std::flush;


        // Print the tokens and their line and column numbers
      for (const Token& token : tokens) {
            std::cout << std::setw(4) << token.line << std::setw(4) << token.column << "  "
                      << std::setw(4) << token.text << std::endl;
        }     
        //std::cout << "After print" << std::endl << std::flush;

         // Create a parser and pass the tokens for parsing
   // Parser parser(tokens);
   // Create a parser and pass the tokens for parsing
   std::cout << "\n";
   for (const Token& token : tokens) {
    std::cout << "Token: " << token.text << " (Line " << token.line << ", Column " << token.column << ")" << std::endl;
}
    std::cout << "\n";
    Parser parser(tokens);
    std::cout << "Start parsing " << std::endl;
    ASTNode* root = parser.parse();
    std::cout << "finish parsing " << std::endl;

    if (root) {
        // Print the AST in infix notation
        std::string infixExpression = parser.printInfix(root);
        std::cout << "Infix Expression: " << infixExpression << std::endl;

        // Evaluate the expression
        double result = root->evaluate();
        std::cout << "Result: " << result << std::endl;
    } else {
        std::cerr << "Failed to parse the input expression." << std::endl;
    }

    } catch (const std::runtime_error& error) {
        // Handle syntax errors
        std::cerr << error.what() << std::endl;
        return 1;
    }

    return 0;
}

