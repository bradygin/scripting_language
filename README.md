# Lightweight Scripting Language Project

## Introduction
The Lightweight Scripting Language Project provides a simple yet powerful solution for working with mathematical expressions. You can input expressions involving basic arithmetic operations, variables, and assignments. The project will parse and evaluate these expressions, allowing you to perform calculations, manipulate variables, and analyze mathematical relationships.

## Compiling the Program
To compile the program:
-Navigate to the project root directory 
-Open a terminal or command prompt
-Type: g++ -o program lib/lexer.cpp lib/infixParser.cpp lib/parser.cpp calc.cpp -std=c++17 -Wall -Wextra -Werror

## Running the Program
After compiling, you can run the program as follows: ./my_program < input.txt

Replace `input.txt` with the path to your input file. The program will parse and evaluate the mathematical expressions in the file.

## Using the Executables
my_program: This is the main program executable. It accepts and processes input files containing mathematical expressions. You can use it to perform calculations, assign values to variables, and more.

Additional executables may be included for specific functionalities or custom extensions of the project.
