# .SILENT:

# Compiler and flags
CXX = ccache g++
CXXFLAGS = -Wall -std=c++23 -I./src -Wno-switch -g -march=native -fsanitize=address

# Directories
SRC_DIR = src
BUILD_DIR = build
BIN_DIR =.

# Executable name
TARGET = main
EXEC_PATH =$(patsubst ./%,%,$(BIN_DIR)/$(TARGET))
EXEC_FULL_PATH =$(CURDIR)/$(patsubst ./%,%,$(BIN_DIR)/$(TARGET))

DESKTOP_FILE_NAME = claire.desktop
DESKTOP_DIR = $(HOME)/.local/share/applications
DESKTOP_PATH = $(DESKTOP_DIR)/$(DESKTOP_FILE_NAME)

# Libraries
LIBS = -lraylib -lGL -lX11 -lpthread -lm -lrt -lbacktrace -lstdc++exp

# Source files (finds all .cpp files recursively in the SRC_DIR)
SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')

# Object files (creates corresponding .o files in the BUILD_DIR)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

# Default target
all: $(EXEC_PATH)

# Create build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $@

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link object files into the final executable
$(EXEC_PATH): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $(BIN_DIR)/$(TARGET) $(LIBS)
	$(MAKE) $(DESKTOP_PATH)

$(DESKTOP_DIR)/$(DESKTOP_FILE_NAME):
	mkdir -p $(DESKTOP_DIR)
	echo "[Desktop Entry]" > $@
	echo "Version=1.0" >> $@
	echo "Name=Claire's Editor" >> $@
	# echo "Comment=My development build of the application" >> $@
	echo "Exec=$(EXEC_FULL_PATH)" >> $@
	echo "Icon=$(CURDIR)/claire_head.png" >> $@
	echo "Terminal=false" >> $@
	echo "Type=Application" >> $@
	echo "Categories=Development;" >> $@
	update-desktop-database $(DESKTOP_DIR) || true

# Clean target to remove build files and the executable
clean:
	rm -rf $(BUILD_DIR) $(EXEC_PATH) $(DESKTOP_PATH)

run: $(TARGET)
	$(EXEC_FULL_PATH)

.PHONY: all clean run
