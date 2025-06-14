.SILENT:

# Compiler and flags
CXX = ccache clang++#g++
SANFLAGS = -fsanitize=undefined
CXXFLAGS = \
	-MMD -MP \
	-std=c++23 \
	-I./src \
	-g \
	-march=native \
	$(SANFLAGS) \
	-fsafe-buffer-usage-suggestions \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wshadow \
	-Wno-switch \
	-Wno-sign-conversion \
	-Wno-unused-parameter \
	-Wno-implicit-float-conversion \
	-Wno-implicit-int-float-conversion \
	-Wno-shorten-64-to-32 \
	-Wno-gnu-zero-variadic-macro-arguments \
	-Wno-float-conversion \
	-Wno-c++11-narrowing
#-fsanitize=memory -fsanitize-memory-track-origins=2
LDFLAGS = \
	$(SANFLAGS) \
	-lraylib \
	-lGL \
	-lX11 \
	-lpthread \
	-lm \
	-lrt \
	-lbacktrace \
	-lstdc++exp

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

SOURCES := $(shell find $(SRC_DIR) -name '*.cpp')
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS	:= $(OBJECTS:.o=.d)

NPROC ?= $(shell nproc || echo 1)
MAKEFLAGS += -j$(NPROC)

.DEFAULT_GOAL := $(EXEC_PATH)
all: $(EXEC_PATH)

# Link object files into the final executable
$(EXEC_PATH): $(OBJECTS)
	@echo "LD    :: $@"
	$(CXX) $^ -o $@ $(LDFLAGS)
	$(MAKE) $(DESKTOP_PATH)

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	@echo "CXX    :: $@"
	$(CXX) $(CXXFLAGS) -c $< -o $@


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

run: $(EXEC_PATH)
	@echo "RUN    :: $(EXEC_FULL_PATH)"
	$(EXEC_FULL_PATH)

.PHONY: all clean run

-include $(DEPS)
