# Makefile for Chess Project

# Compiler and flags
CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -Iinclude -Iinclude/update
LDFLAGS = -lSDL3 -lm

# Directories
SRC_DIR    = src
BUILD_DIR  = target
TARGET     = chess

# Source files (all .c files including update folder)
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')

# Object files (mirror folder structure in target/)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Extract directories for object files
DIRS := $(sort $(dir $(OBJS)))

# Default target
all: $(TARGET)

# Link final executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile each object
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Ensure all needed directories exist
$(BUILD_DIR):
	@for d in $(DIRS); do mkdir -p $$d; done

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

# Phony targets
.PHONY: all clean
