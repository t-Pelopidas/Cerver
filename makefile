SHELL := /bin/bash

CC = clang
CFLAGS = -Wall -Wextra -I./include -fsanitize=address -fno-omit-frame-pointer

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
	$(CC) $(CFLAGS) -c $< -o $@ -g

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean up build artifacts
clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	@./$(TARGET)

# Start the server, send it the test request, then report how the server exited.
# curl exits 52 until the server sends a response, so only the server's status counts.
test: $(TARGET)
	@./$(TARGET) & server=$$!; \
	sleep 0.5; \
	if ! kill -0 $$server 2>/dev/null; then wait $$server; exit $$?; fi; \
	curl -s -m 5 -o /dev/null http://127.0.0.1:8000/ \
	  -H 'User-Agent:' -H 'Accept:' \
	  -H 'Colons: a:b: c' \
	  -H $$'Tab:\ttabbed' \
	  -H 'NoSpace:value' \
	  -H 'Empty;' \
	  -H 'After: x' \
	  -H 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA: v' \
	  -H 'BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB: v' \
	  -H 'CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC: v' \
	  -H 'DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD: v' \
	  -H 'L: vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv'; \
	if [ $$? -eq 28 ]; then kill $$server; fi; \
	wait $$server

.PHONY: all clean run test
