#ifndef KB_INPUT_H
#define KB_INPUT_H

#include "structs.h"
#include <libgen.h>
#include <ncurses.h>
#include <stdio.h>

#define MAX_HISTORY 256

void handleInputScrollUp(int* highlight, int* scroll_position);
void handleInputScrollDown(int* highlight, int* scroll_position, int* item_count, int* height);
void handleInputToggleHidden(int* show_hidden, int* scroll_position, int* highlight);
void handleInputMovCursBtm(int* highlight, int* item_count, int* scroll_position, int* max_y);
int find_item(const char *query, FileItem items[], int *item_count, int *start_index, int direction);
/* ------------------------------
 * void handleInputScopeBack(int* history_count, int* highlight, int* scroll_position, const char*
 * current_path, DirHistory history[]); void handleInputScopeForward(WINDOW *win, WINDOW *info_win,
 * int *history_count, int *highlight, int *scroll_position, bool *firstKeyPress, FileItem items[],
 * DirHistory history[], const char* cur_user, const char* current_path);
 * ----------------------------- */

#endif
