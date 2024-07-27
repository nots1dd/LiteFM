#// // // // // // 
#//             //
#//   LITE FM   //
#//             //
#// // // // // // 

# BY nots1dd 

# Makefile CONFIGURATION FOR LITE FILE MANAGER

# LICENSED UNDER GNU GPL v3

# Variables
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99
LDFLAGS = $(CURSES_LIBRARIES) $(LIBARCHIVE_LIBRARIES)
INCLUDE_DIRS = -I/usr/include -I/usr/local/include -I$(CMAKE_SOURCE_DIR)
LIB_DIRS = -L/usr/lib -L/usr/local/lib

# Source files
SRC = lfm.c
OBJ = $(SRC:.c=.o)

# Executable name
TARGET = litefm

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS) $(LIB_DIRS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(TARGET) $(OBJ)

.PHONY: all clean install
