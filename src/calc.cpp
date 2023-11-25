#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include "lib/lexer.h"
#include "lib/token.h"
#include "lib/infixParser.h"

int main() {
    Lexer lexer(std::cin);
    
    try {
        std::vector<Token> tokens = lexer.tokenize();
        infixParser parser(tokens);
        //double result = 0.0;
        while (auto root = parser.infixparse()) {              
            std::vector<std::string> results = parser.evaluateToArray(root);
            if (results.empty()) continue;
            if (results.size() == 1) std::cout << results[0] << std::endl;
            else {
                std::cout << "[";
                int size = (int)results.size();
                int count = 0;
                for (auto result : results) {
                    std::cout << result;
                    if (++count < size) std::cout << ", ";
                    else std::cout << "]\n";
                }
            }
        }
        //std::cout << result << std::endl;
    } catch (const SyntaxError& error) {
        std::cout << error.what() << std::endl;
        return 1;
    } catch (const UnexpectedTokenException& error) {
        std::cout << error.what() << std::endl;
        return 2;
    }catch (const InvalidOperandTypeException& error) {
        std::cout << error.what() << std::endl;
        return 3;
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    return 0;
}
