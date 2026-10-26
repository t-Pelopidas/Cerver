CC = gcc
CFLAGS = -Wall -Wextra -I./include

SRC_DIR = src
BUILD_DIR = build

# Automatically find all .c files in the src directory
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Generate corresponding .o file paths in the build directory
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Name of your final executable
TARGET = $(BUILD_DIR)/app

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -g
	rm $(OBJS)

# Compile source files into object files, requiring the build directory to exist
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)

run:
	@./$(TARGET)

.PHONY: all clean
