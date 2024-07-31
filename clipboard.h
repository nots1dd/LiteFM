#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LENGTH 256

void yank_selected_item(char *selected_item);

#endif
