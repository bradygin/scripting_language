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
        //std::cout << "Before tokenize()" << std::endl << std::flush;

        std::vector<Token> tokens = lexer.tokenize();

        //std::cout << "After tokenize()" << std::endl << std::flush;


        // Print the tokens and their line and column numbers
      for (const Token& token : tokens) {
            std::cout << token.line << std::setw(3) << token.column << " "
                      << std::setw(2) << token.text << std::endl;
        }     
        //std::cout << "After print" << std::endl << std::flush;

    } 
    catch (const std::runtime_error& error) {
        // Handle syntax errors
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
