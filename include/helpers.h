#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>
#include <string.h>

// Define ANSI escape codes for colors
#define COLOR_RESET      "\033[0m"
#define COLOR_BLUE       "\033[1;34m"
#define COLOR_GREEN      "\033[1;32m"
#define COLOR_RED        "\033[1;31m"

void show_help();
void show_version();

#endif
