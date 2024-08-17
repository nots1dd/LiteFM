#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H

#include <stdio.h>
#include <yaml.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>

#include "hashtable.h"

// Function prototypes
bool load_syntax(const char *path, HashTable *keywords, HashTable *singlecomments, HashTable *multicomments1, HashTable *multicomments2, HashTable *strings, HashTable *functions, HashTable *symbols, HashTable *operators, int *singlecommentslen);
void highlight_code(WINDOW *win, int start_y, int start_x, const char *code, HashTable *keywords, HashTable *singlecomments, HashTable *multicomments1, HashTable *multicomments2, HashTable *strings, HashTable *functions, HashTable *symbols, HashTable *operators, int *singlecommentslen);

#endif
