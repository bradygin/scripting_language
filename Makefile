# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror

# Source and Object Files
 MAIN_SRC = src/format.cpp
# MAIN_SRC = src/scrypt.cpp
# MAIN_SRC = src/calc.cpp
# MAIN_SRC = src/parse.cpp
# MAIN_SRC = src/lex.cpp




LIB_SRC = src/lib/lexer.cpp src/lib/infixParser.cpp src/lib/parser.cpp
SRC = $(MAIN_SRC) $(LIB_SRC)
OBJ = $(SRC:.cpp=.o)

# Compile and Link
all: program

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

program: $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Clean
clean:
	rm -f $(OBJ) program
