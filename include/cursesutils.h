// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        cursesutils.h
 *
 *  Description: This header file provides utility functions for managing
 *                NCurses windows and performing various terminal operations
 *                used within the LiteFM file manager application. It includes
 *                functionalities for handling user input, displaying messages,
 *                initializing colors, and creating windows with custom borders.
 *
 *  Author:      Siddharth Karanam
 *  Created:     31/07/24
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       The functions declared in this header facilitate operations
 *                such as showing messages, handling user inputs, and
 *                creating styled windows within the LiteFM application.
 *                Ensure that NCurses is properly initialized before using
 *                these utilities.
 *
 *  Revision History:
 *      31/07/24 - Initial creation.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef CURSESUTILS_H
#define CURSESUTILS_H

#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* Constants */
#define MAX_PATH_LENGTH 256

/* COLOR PAIRS DEF */

#define FILE_COLOR_PAIR 1
#define DIR_COLOR_PAIR 2
#define ARCHIVE_COLOR_PAIR 3
#define AUDIO_COLOR_PAIR 4
#define IMAGE_COLOR_PAIR 5
#define SYMLINK_COLOR_PAIR 6
#define AQUA_COLOR_PAIR 7
#define VIOLET_COLOR_PAIR 8
#define DARK_BG_COLOR_PAIR 9
#define MAGENTA_COLOR_PAIR 10
#define SPECIAL_COLOR_PAIR 11
#define ERROR_COLOR_PAIR 12
#define CUSTOM_COLOR_PAIR 13
#define TITLE_COLOR_PAIR 14
#define HIGHLIGHT_COLOR_PAIR 15

/* COMPRESSION OPTIONS COLOR PAIR + MISC DEF */

#define STEP_SELECT_FORMAT 0
#define STEP_SELECT_ACTION 1

#define COMP_COLOR_TITLE 31
#define COMP_COLOR_HIGHLIGHT 32
#define COMP_COLOR_NORMAL 33
#define COMP_COLOR_FOOTER 34

#define OPTION_TAR 1
#define OPTION_ZIP 2
#define OPTION_EXIT 0

// Function Prototypes
void    show_message(WINDOW* win, const char* message);
void    print_limited(WINDOW* win, int y, int x, const char* str);
void    clearLine(WINDOW* win, int x, int y);
void    colorLine(WINDOW* win, const char* info, int colorpair, int x, int y);
int     show_compression_options(WINDOW* parent_win);
void    show_term_message(const char* message, int err);
void    init_curses();
void    draw_colored_border(WINDOW* win, int color_pair);
void    init_custom_color(short color_index, int r, int g, int b);
void    color_pair_init();
int     confirm_action(WINDOW* win, const char* message);
void    draw_3d_info_win(WINDOW* win, int y, int x, int height, int width, int color_pair,
                         int shadow_color_pair);
void    get_user_input_from_bottom(WINDOW* win, char* buffer, int max_length, const char* type,
                                   const char* current_path);
void    get_user_input(WINDOW* win, char* input, int max_length);
void    displayHelp(WINDOW* main_win);
WINDOW* create_centered_window(int height, int width);
void    check_term_size(WINDOW* win, WINDOW* info_win);
void    displayProgressWindow(WINDOW* progress_win, FILE* progress_data);

#endif // CURSESUTILS_H
