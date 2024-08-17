#// // // // // // 
#//             //
#//   LITE FM   //
#//             //
#// // // // // // 

#/*
# * ---------------------------------------------------------------------------
# *  File:        Makefile
# *
# *  Description: Default Makefile for LiteFM
# *
# *  Author:      Siddharth Karanam
# *  Created:     31/07/24
# * 
# *  Copyright:   2024 nots1dd. All rights reserved.
# * 
# *  License:     <GNU GPL v3>
# * 
# *  Revision History:
# *      31/07/24 - Initial creation.
# *
# * ---------------------------------------------------------------------------
# */

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c99
LDFLAGS =

# Libraries and include directories
CURSES_LIBS = $(shell pkg-config --libs ncurses)
CURSES_INCS = $(shell pkg-config --cflags ncurses)
ARCHIVE_LIBS = $(shell pkg-config --libs libarchive)
ARCHIVE_INCS = $(shell pkg-config --cflags libarchive)
YAML_LIBS = $(shell pkg-config --libs libyaml)
YAML_INCS = $(shell pkg-config --cflags libyaml)

# Source files
SRCS = lfm.c \
       src/cursesutils.c \
       src/filepreview.c \
       src/dircontrol.c \
       src/archivecontrol.c \
       src/clipboard.c \
       src/logging.c \
       src/signalhandling.c \
       src/highlight.c \
       src/hashtable.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = litefm

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(CURSES_LIBS) $(ARCHIVE_LIBS) $(YAML_LIBS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) $(CURSES_INCS) $(ARCHIVE_INCS) $(YAML_INCS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean
