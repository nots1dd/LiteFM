// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#ifndef CURSESUTILS_H
#define CURSESUTILS_H

#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Constants
#define MAX_PATH_LENGTH 256

// Function Prototypes
void show_message(WINDOW *win, const char *message);
void clearLine(WINDOW *win, int x, int y);
void colorLine(WINDOW* win, const char* info, int colorpair, int x, int y);
int show_compression_options(WINDOW *parent_win);
void show_term_message(const char *message, int err);
void init_curses();
void draw_colored_border(WINDOW *win, int color_pair);
void init_custom_color(short color_index, int r, int g, int b);
void color_pair_init();
int confirm_action(WINDOW *win, const char *message);
void draw_3d_info_win(WINDOW *win, int y, int x, int height, int width, int color_pair, int shadow_color_pair);
void get_user_input_from_bottom(WINDOW *win, char *buffer, int max_length, const char* type);
void get_user_input(WINDOW *win, char *input, int max_length);
void displayHelp(WINDOW* main_win);
WINDOW *create_centered_window(int height, int width);

#endif // CURSESUTILS_H
