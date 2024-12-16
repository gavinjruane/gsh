# Compiler and Flags
CC = gcc
CFLAGS = -Iinclude -Wall -pedantic -g -std=c99

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Source and Object Files
SRCS = $(SRC_DIR)/gsh.c $(SRC_DIR)/builtin.c

# Output Executeable
TARGET = gsh
OBJECT_FILE = gsh

# Default Target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(SRCS) $(CFLAGS) -o $@

clean:
	rm -f $(OBJECT_FILE) $(TARGET)
