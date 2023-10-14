#include "lexer.h"
#include "token.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

int main() {
    std::cout << "Enter your input:" << std::endl;
    // Initialize the lexer with standard input (cin)
    Lexer lexer(std::cin);

    try {
        // Create a vector of tokens by calling the lexer's tokenize function
        std::vector<Token> tokens = lexer.tokenize();
        // Print the tokens and their line and column numbers
        for (const Token& token : tokens) {
            std::cout << std::setw(4) << token.line << std::setw(4) << token.column << "  "
                      << std::setw(4) << token.text << std::endl;
        }
        
    } catch (const std::runtime_error& error) {
        // Handle syntax errors
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
