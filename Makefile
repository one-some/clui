.SILENT:

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -std=c++17 -I./src -Wno-switch -g -fsanitize=address

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR =.

# Executable name
TARGET = main

# Libraries (add your raylib libraries here)
LIBS = -lraylib -lGL -lX11 -lpthread -lm -lrt

# Source files (finds all .cpp files recursively in the SRC_DIR)
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Object files (creates corresponding .o files in the BUILD_DIR)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(BIN_DIR)/$(TARGET)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into the final executable
$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$(TARGET) $(LIBS)

# Clean target to remove build files and the executable
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(TARGET)

run: $(TARGET)
	./main

.PHONY: all clean
