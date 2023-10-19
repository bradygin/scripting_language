#include "lib/lexer.h"
#include "lib/token.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

int main() {
    // Initialize the lexer with standard input (cin)
    Lexer lexer(std::cin);

    try {
        /* Create a vector of tokens by calling the lexer's tokenize() member function which uses 
        the nextToken() helper function to create tokens and then pushes them into a vector */

        std::vector<Token> tokens = lexer.tokenize();

        // Print the tokens and their line and column numbers
    for (const Token& token : tokens) {
            std::cout << std::setw(4) << std::right << token.line 
            << std::setw(5) << std::right << token.column << " "
            << std::setw(6) << std::left << token.text << std::setw(0) << std::endl;
        }
    }
    catch (const std::runtime_error& error) {
        // Handle syntax errors
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
