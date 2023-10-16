# Compiler and flags
CC = g++
CFLAGS = -std=c++17 -Wall -Wextra -Werror -Isrc/lib

# Directories
SRC_DIR = src/lib

# Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst %.cpp,%.o,$(SRC_FILES))
MAIN_FILE = src/lex.cpp
EXE_FILE = my_program

# Main target
all: $(EXE_FILE)

# Linking
$(EXE_FILE): $(OBJ_FILES) $(MAIN_FILE)
	$(CC) $(CFLAGS) $^ -o $@

# Compiling
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(SRC_DIR)/*.o $(EXE_FILE)
