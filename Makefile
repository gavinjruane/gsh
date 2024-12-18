# Compiler and Flags
CC = gcc
CFLAGS = -Iinclude -Wall -pedantic -g -std=c99

# Directories
SRC_DIR = src
INCLUDE_DIR = include

# Source and Object Files
SRCS = $(SRC_DIR)/gsh.c $(SRC_DIR)/builtin.c $(SRC_DIR)/internal.c
OBJS = $(SRCS: .c=.o)

# Output Executeable
TARGET = gsh
OBJECT_FILE = gsh

# Default Target
all: $(TARGET)

# Link final executeable
$(TARGET): $(OBJS)
	$(CC) $(SRCS) $(CFLAGS) -o $@

# Create individual object files
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# $< refers to the source file
# $@ refers to the object file

clean:
	rm -f $(OBJECT_FILE) $(TARGET)
