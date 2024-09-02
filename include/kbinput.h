// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        kbinput.h
 *  Description: Function calls when a keybind is pressed are here.
 *
 *  Author:      Siddharth Karanam
 *  Created:     <29/08/24>
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:      Some function calls that require a global scope need
 *              proper implementation
 *
 *  Revision History:
 *      <29/08/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef KB_INPUT_H
#define KB_INPUT_H

#include "structs.h"
#include <libgen.h>
#include <ncurses.h>
#include <stdio.h>

#define MAX_HISTORY    256
#define UNICODE_SEARCH "🔍"

void handleInputScrollUp(int* highlight, int* scroll_position);
void handleInputScrollDown(int* highlight, int* scroll_position, int* item_count, int* height);
void handleInputToggleHidden(int* show_hidden, int* scroll_position, int* highlight);
void handleInputMovCursBtm(int* highlight, int* item_count, int* scroll_position, int* max_y);
void handleInputMovCursTop(int* highlight, int* scroll_position);
void handleInputGoToDir(const char* current_path, const char* path, int* highlight,
                        int* scroll_position);
void handleInputRename(int* item_count, int* highlight, int* scroll_position,
                       const char* current_path, FileItem items[]);
int  find_item(const char* query, FileItem items[], int* item_count, int* start_index,
               int direction);
void handleInputStringSearch(WINDOW* win, FileItem items[], int* item_count, int* highlight,
                             int* scroll_position, int* height, char* last_query,
                             const char* current_path);
void handleInputStringOccurance(int direction, const char* last_query, FileItem items[],
                                int* item_count, int* highlight, int* scroll_position, int* height);
void handleInputExtractArchive(WINDOW* win, FileItem items[], const char* current_path,
                               const char* last_query, int* scroll_position, int* highlight);
void handleInputCompressInode(WINDOW* win, FileItem items[], const char* current_path,
                              int* highlight, int* scroll_position);
/* ------------------------------
 * void handleInputScopeBack(int* history_count, int* highlight, int* scroll_position, const char*
 * current_path, DirHistory history[]); void handleInputScopeForward(WINDOW *win, WINDOW *info_win,
 * int *history_count, int *highlight, int *scroll_position, bool *firstKeyPress, FileItem items[],
 * DirHistory history[], const char* cur_user, const char* current_path);
 * ----------------------------- */

#endif
