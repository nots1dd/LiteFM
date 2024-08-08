// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include "../include/cursesutils.h"

void draw_3d_info_win(WINDOW *win, int y, int x, int height, int width, int color_pair, int shadow_color_pair);

void show_message(WINDOW *win, const char *message) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    mvwprintw(win, maxy - 2, 2, message);
    wrefresh(win);
}

void clearLine(WINDOW *win, int x, int y) {
  mvwprintw(win, x, y, "\n");
}


void print_limited(WINDOW *win, int y, int x, const char *str) {
    int max_x = getmaxx(win) - 6; // Get max width of window minus borders
    int available_width = max_x - x; // Available width from current x position

    if (available_width <= 3) {
        mvwprintw(win, y, x, "..."); // If available width is too small to display anything but ellipsis
        return;
    }

    char buffer[available_width + 1]; // +1 for the null terminator
    size_t len = strlen(str);

    if (len > available_width - 3) { // Check if truncation is needed
        // Copy and truncate the string, leaving space for "..."
        strncpy(buffer, str, available_width - 3);
        buffer[available_width - 3] = '\0'; // Null terminate
        strcat(buffer, "..."); // Append ellipsis
    } else {
        // No truncation needed, just copy the string
        strncpy(buffer, str, available_width);
        buffer[available_width] = '\0'; // Null terminate
    }

    mvwprintw(win, y, x, " %s ", buffer);
}


void colorLine(WINDOW* win, const char* info, int colorpair, int x, int y) {
  wattron(win, COLOR_PAIR(colorpair));
  mvwprintw(win, x, y, info);
  wattroff(win, COLOR_PAIR(colorpair));
} 

int show_compression_options(WINDOW *parent_win) {
    WINDOW *options_win;
    int choice;
    int highlight = 0;
    int c;
    int step = 0; // 0 for selecting format, 1 for selecting action

    // Define window size and position
    int win_height = 10;
    int win_width = 40;
    int win_y = (LINES - win_height) / 2;
    int win_x = (COLS - win_width) / 2;

    // Create a new window for displaying options
    options_win = newwin(win_height, win_width, win_y, win_x);
    box(options_win, 0, 0);
    keypad(options_win, TRUE); // Enable special keys (e.g., arrow keys)
    draw_3d_info_win(options_win, win_y, win_x, win_height, win_width, 2, 4);

    // Define colors
    init_pair(31, COLOR_BLACK, COLOR_BLUE);    // Title
    init_pair(32, COLOR_BLACK, COLOR_GREEN);  // Highlighted option
    init_pair(33, COLOR_WHITE, COLOR_BLACK);   // Normal text
    init_pair(34, COLOR_RED, COLOR_BLACK);      // Footer text

    const char *top_options[] = {
        "TAR (.tar)",
        "ZIP (.zip)"
    };
    const char *bottom_options[] = {
        "COMPRESS",
        "EXIT"
    };

    int num_top_options = sizeof(top_options) / sizeof(top_options[0]);
    int num_bottom_options = sizeof(bottom_options) / sizeof(bottom_options[0]);
    char title_buf[60];

    while (1) {
        // Clear the window
        werase(options_win);

        // Draw the box again
        box(options_win, 0, 0);

        if (step == 0) {
            // Display title
            wattron(options_win, COLOR_PAIR(31));
            mvwprintw(options_win, 1, (win_width - strlen(" Compression format: ")) / 2, " Compression format: ");
            wattroff(options_win, COLOR_PAIR(31));

            // Display top options
            for (int i = 0; i < num_top_options; ++i) {
                if (i == highlight) {
                    wattron(options_win, COLOR_PAIR(32) | A_BOLD);
                } else {
                    wattron(options_win, COLOR_PAIR(33));
                }
                mvwprintw(options_win, i + 3, (win_width - strlen(top_options[i])) / 2, "%s", top_options[i]);
                wattroff(options_win, COLOR_PAIR(32) | COLOR_PAIR(33) | A_BOLD);
            }

            // Display bottom options
            int bottom_y = win_height - 2;
            int left_x = 2;
            int right_x = win_width - strlen(bottom_options[1]) - 2;

            // Left bottom option (COMPRESS)
            wattron(options_win, COLOR_PAIR(33));
            mvwprintw(options_win, bottom_y, left_x, "%s", bottom_options[0]);
            wattroff(options_win, COLOR_PAIR(33));

            // Right bottom option (EXIT)
            wattron(options_win, COLOR_PAIR(33));
            mvwprintw(options_win, bottom_y, right_x, "%s", bottom_options[1]);
            wattroff(options_win, COLOR_PAIR(33));

            // Refresh and wait for user input
            wrefresh(options_win);
            c = wgetch(options_win); 

            switch (c) {
                case KEY_UP:
                    if (highlight > 0) {
                        highlight--;
                    }
                    break;
                case KEY_DOWN:
                    if (highlight < num_top_options - 1) {
                        highlight++;
                    }
                    break;
                case 10: // Enter key
                    step = 1; // Move to next step
                    snprintf(title_buf, 60, " Select %s action: ", top_options[highlight]);
                    choice = highlight + 1;
                    highlight = num_top_options; // Set default highlight to COMPRESS
                    break;
                case 27: // ESC key
                    delwin(options_win);
                    refresh(); // Refresh the main window to ensure no artifacts remain
                    return -1;
            }
        } else if (step == 1) {
            // Display title
            wattron(options_win, COLOR_PAIR(31)); 
            mvwprintw(options_win, 1, (win_width - 30), title_buf);
            wattroff(options_win, COLOR_PAIR(31));

            // Display top options
            for (int i = 0; i < num_top_options; ++i) {
                if (i == highlight) {
                    wattron(options_win, COLOR_PAIR(32) | A_BOLD);
                } else {
                    wattron(options_win, COLOR_PAIR(33));
                }
                mvwprintw(options_win, i + 3, (win_width - strlen(top_options[i])) / 2, "%s", top_options[i]);
                wattroff(options_win, COLOR_PAIR(32) | COLOR_PAIR(33) | A_BOLD);
            }

            // Display bottom options
            int bottom_y = win_height - 2;
            int left_x = 2;
            int right_x = win_width - strlen(bottom_options[1]) - 2;

            // Left bottom option (COMPRESS)
            if (highlight == num_top_options) {
                wattron(options_win, COLOR_PAIR(32) | A_BOLD);
            } else {
                wattron(options_win, COLOR_PAIR(33));
            }
            mvwprintw(options_win, bottom_y, left_x, "%s", bottom_options[0]);
            wattroff(options_win, COLOR_PAIR(32) | COLOR_PAIR(33) | A_BOLD);

            // Right bottom option (EXIT)
            if (highlight == num_top_options + 1) {
                wattron(options_win, COLOR_PAIR(32) | A_BOLD);
            } else {
                wattron(options_win, COLOR_PAIR(33));
            }
            mvwprintw(options_win, bottom_y, right_x, "%s", bottom_options[1]);
            wattroff(options_win, COLOR_PAIR(32) | COLOR_PAIR(33) | A_BOLD);

            // Refresh and wait for user input
            wrefresh(options_win);
            c = wgetch(options_win);

            switch (c) {
                case KEY_LEFT:
                    if (highlight == num_top_options + 1) {
                        highlight = num_top_options; // Move to COMPRESS
                    }
                    break;
                case KEY_RIGHT:
                    if (highlight == num_top_options) {
                        highlight = num_top_options + 1; // Move to EXIT
                    }
                    break;
                case 10: // Enter key
                    if (highlight == num_top_options) {
                        // Handle compression logic
                        delwin(options_win);
                        refresh(); // Refresh the main window to ensure no artifacts remain
                        if (choice == 1) {
                          return 1; // return 1 (compression type: tar)
                        } else {
                          return 2; // return 2 (compression type: zip)
                        }
                    } else if (highlight == num_top_options + 1) {
                        delwin(options_win);
                        refresh(); // Refresh the main window to ensure no artifacts remain
                        return 0; // Indicate "EXIT" was chosen
                    }
                    break;
                case 27: // ESC key
                    delwin(options_win);
                    refresh(); // Refresh the main window to ensure no artifacts remain
                    return -1;
            }
        }
    }

    delwin(options_win);
    wrefresh(parent_win);
}



void show_term_message(const char *message, int err) { 
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    int message_y = maxy - 1;  // Adjust position as needed
    move(message_y, 0);
    clrtoeol();

    move(message_y, 0);
    clrtoeol();
    refresh();
    
    if (err==1) {
      attron(COLOR_PAIR(12));
      mvprintw(message_y, 0, " \u2718 %s", message); // \u2718 is unicode for ✘
      attroff(COLOR_PAIR(12));
    } else if (err==0) {
      attron(COLOR_PAIR(1));
      mvprintw(message_y, 0, " \u2714 %s", message); // \u2714 is unicode for ✔
      attroff(COLOR_PAIR(1));
    } else if (err=-1) {
      attron(COLOR_PAIR(4));
      mvprintw(message_y, 0, "%s", message);
      attroff(COLOR_PAIR(4));
    }
    refresh();
    
}

void init_curses() {
    setlocale(LC_ALL, "");
    initscr();
    start_color();  // Enable color
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);
}

void draw_colored_border(WINDOW *win, int color_pair) {
    wattron(win, COLOR_PAIR(color_pair));
    box(win, 0, 0); // Draw the border using box function
    wattroff(win, COLOR_PAIR(color_pair));
    wrefresh(win);
}

void init_custom_color(short color_index, int r, int g, int b) {
    // Convert RGB from 0-255 to 0-1000 scale for ncurses
    short red = (short)(r * 1000 / 255);
    short green = (short)(g * 1000 / 255);
    short blue = (short)(b * 1000 / 255);
    init_color(color_index, red, green, blue);
}

void color_pair_init() { 
  // Define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // File color (Green)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);   // Directory color (Blue)
    init_pair(3, 167, 235);    // Unextracted archives (Red)
    init_pair(4, 175, 235); // Audio files (Pink)
    init_pair(5, 214, 235); // Image files (yellow)
    init_pair(6, 108, 235); // bright blue gruvbox type
    init_pair(7, 108, 235); // aqua gruvbox
    init_pair(8, 61, 234); // violet base03
    init_pair(9, 234, 230);
    // init_pair(9, 235, 223); // gruvbox dark bg, white fg
    init_pair(10, 125, 234); // magenta
    init_pair(11, 168, COLOR_BLACK);
    init_pair(12, COLOR_RED, COLOR_BLACK); // Regular color red
    init_custom_color(13, 0xDC, 0x9A, 0x1F); // #BDAE93 #DC9A1F
    init_pair(14, 235, 175); // ping bg, black fg
    init_pair(15, 175, 175);
    init_pair(20, COLOR_BLUE + 1, COLOR_BLACK); // Keywords
    init_pair(21, COLOR_GREEN + 1, COLOR_BLACK); // Comments
    init_pair(22, COLOR_YELLOW, COLOR_BLACK); // Numbers
    init_pair(23, COLOR_CYAN, COLOR_BLACK);   // Strings
    init_pair(24, COLOR_WHITE, COLOR_BLACK);  // Default text
}

int confirm_action(WINDOW *win, const char *message) {
    int confirm_win_height = 8;
    int confirm_win_width = (COLS / 3);
    int confirm_win_y = (LINES - confirm_win_height) / 2;
    int confirm_win_x = (COLS - confirm_win_width) / 2;

    WINDOW *confirm_win = newwin(confirm_win_height, confirm_win_width, confirm_win_y, confirm_win_x);
    draw_colored_border(confirm_win, 5);

    // Create a buffer for the truncated message
    char truncated_message[confirm_win_width - 3 + 1];
    if (strlen(message) > confirm_win_width - 3) {
        strncpy(truncated_message, message, confirm_win_width - 3 - 3);
        strcpy(truncated_message + confirm_win_width - 3 - 3, "...");
    } else {
        strcpy(truncated_message, message);
    }
    
    colorLine(confirm_win, " Are you sure?? ", 14, 0, 2);

    wattron(confirm_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(confirm_win, 2, 2, "%s", truncated_message);
    wattroff(confirm_win, A_BOLD | COLOR_PAIR(3));
    mvwprintw(confirm_win, 4, 2, "Press 'y' to confirm, 'n' to cancel.");
    mvwprintw(confirm_win, 6, 2, "Confirming the actions of this window CANNOT be reverted!");
    wrefresh(confirm_win);

    int ch = wgetch(confirm_win);
    delwin(confirm_win);

    return ch == 'y' || ch == 'Y' || ch == 10;
}


void draw_3d_info_win(WINDOW *win, int y, int x, int height, int width, int color_pair, int shadow_color_pair) {
    // Create shadow window
    WINDOW *shadow_win = newwin(height, width, y + 1, x + 1);
    wattron(shadow_win, COLOR_PAIR(shadow_color_pair)); 
    wattroff(shadow_win, COLOR_PAIR(shadow_color_pair));
    draw_colored_border(shadow_win, 4);
    wrefresh(shadow_win);
}

void truncate_path(char *path) {
    char *last_slash = strrchr(path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';  // Replace the last '/' with '\0' to truncate the string
    }
}

void get_user_input_from_bottom(WINDOW *win, char *buffer, int max_length, const char* type, const char *current_path) {
    int y, x;
    getmaxyx(stdscr, y, x);  // Get screen dimensions

    echo();
    nodelay(win, FALSE);
    attron(A_BOLD);  // Turn on bold attribute
    if (strcmp(type, "search") == 0) {
        attron(COLOR_PAIR(3));
        mvprintw(y - 1, 0, " /");
        attroff(COLOR_PAIR(3));
    } else if (strcmp(type, "rename") == 0) {
        attron(COLOR_PAIR(2));
        mvprintw(y - 1, 0, " Rename to: ");
        attroff(COLOR_PAIR(2));
    } else if (strcmp(type, "add") == 0) {
        attron(COLOR_PAIR(2));
        mvprintw(y - 1, 0, " Add: ");
        attroff(COLOR_PAIR(2));
    } else if (strcmp(type, "move") == 0) {
        attron(COLOR_PAIR(2));
        mvprintw(y-1,0, " Move to: ");
        attroff(COLOR_PAIR(2));
    } else if (strcmp(type, "goto") == 0) {
      attron(COLOR_PAIR(2));
      mvprintw(y-1,0," Go to: %s/",current_path);
      attroff(COLOR_PAIR(2));
    }
    attroff(A_BOLD);  // Turn off bold attribute
    clrtoeol();  // Clear the rest of the line to handle previous content

    // Move the cursor to the appropriate location in the window
    if (strcmp(type, "search") == 0) {
      wmove(win, getmaxy(win) - 1, 2);
    } else if (strcmp(type, "rename") == 0 || strcmp(type, "move") == 0) {
      wmove(win, getmaxy(win) - 1, 12);
    } else if (strcmp(type, "add") == 0) {
      wmove(win, getmaxy(win) - 1, 6);
    } else if (strcmp(type, "goto") == 0) {
      wmove(win, getmaxy(win) - 1, 9+strlen(current_path));
    }
    else {
      wmove(win, getmaxy(win) - 1, 1);
    }
    attron(COLOR_PAIR(3));
    if (strcmp(type, "goto") == 0) {
      char tmp_buf[256];
      wgetnstr(win, tmp_buf, max_length);
      if (strcmp(tmp_buf, "..") == 0) {
        truncate_path(current_path);
        snprintf(buffer, max_length, "%s", current_path);
      } else {
        snprintf(buffer, max_length, "%s/%s", current_path, tmp_buf);
      }
    } else {
      wgetnstr(win, buffer, max_length);
    }
    noecho();
    nodelay(win, TRUE);

    mvprintw(y - 1, 0, " ");  // Clear the prompt after getting input
    clrtoeol();  // Clear the rest of the line to handle previous content
    attroff(COLOR_PAIR(3));
    refresh();
}


void get_user_input(WINDOW *win, char *input, int max_length) {
    echo();
    nocbreak();
    curs_set(1);

    mvwgetnstr(win, 1, 2, input, max_length - 1);

    noecho();
    cbreak();
    curs_set(0);
}

void displayHelp(WINDOW* main_win) {
    int help_win_height = LINES - 15;
    int help_win_width = (COLS / 3);
    int help_win_y = (LINES - help_win_height) / 4;
    int help_win_x = (COLS - help_win_width) / 2;
    WINDOW* help_win = newwin(help_win_height, help_win_width, help_win_y, help_win_x);
    keypad(help_win, TRUE); // Enable keyboard input for the help window

    // Draw the window borders and decorations
    draw_colored_border(help_win, 4);
    draw_3d_info_win(help_win, help_win_y, help_win_x, help_win_height, help_win_width, 1, 2);

    // Print the header
    wattron(help_win, A_BOLD | COLOR_PAIR(14));
    mvwprintw(help_win, 1, (help_win_width - 11) / 2, " Help Window ");
    wattroff(help_win, A_BOLD | COLOR_PAIR(14));

    const char *commands[] = {
        " Scroll up            - [k/UP_ARROW]",
        " Scroll down          - [j/DOWN_ARROW]",
        " Go inside sel. dir   - [l/RIGHT_ARROW/ENTER]",
        " Go to parent dir     - [h/LEFT_ARROW]",
        " String search        - [/]",
        " String next match    - [n]",
        " String prev match    - [N]",
        " Go to top of list    - [gg]",
        " Go to end of list    - [G]",
        " Yank file name       - [y]",
        " Yank file location   - [Y]",
        " Add a new file/dir   - [a]",
        " Delete file/dir      - [d] {NON-RECURSIVE DIR DELETE!}",
        " Recursive dir delete - [D]",
        " Rename a file/dir    - [R]",
        " Extract archive      - [E] {Works for .zip, {.tar.}, .7z}",
        " Compress directory   - [Z] {Works for .zip and .tar ONLY!}",
        " Move a file/dir      - [M]",
        " Show help win        - [?]",
        " Go to / directory    - [H]",
        " Go to ~ (home) dir   - [gh]",
        " Go to input dir      - [gt]",
        " Get help prompt      - [?]",
    };
    int num_commands = sizeof(commands) / sizeof(commands[0]);
    int highlight = 0;
    int c;

    while (1) {
        // Display commands
        for (int i = 0; i < num_commands; ++i) {
            if (i == highlight) {
                wattron(help_win, A_REVERSE);
            }
            if (i == 11 || i == 18) { // Highlighting section headers
                wattron(help_win, A_BOLD | COLOR_PAIR(3));
                mvwprintw(help_win, i + 3, 2, i == 11 ? " --------- FILE CMDS --------" : " --------- NAVIGATION --------");
                wattroff(help_win, A_BOLD | COLOR_PAIR(3));
            } else {
                mvwprintw(help_win, i + 4, 2, "%s", commands[i]);
            }
            wattroff(help_win, A_REVERSE);
        }

        // Display footer
        wattron(help_win, A_BOLD | COLOR_PAIR(3));
        mvwprintw(help_win, help_win_height - 2, (help_win_width - 29) / 2, "Press 'q' to exit this help");
        wattroff(help_win, A_BOLD | COLOR_PAIR(3));

        // Refresh and wait for user input
        wrefresh(help_win);
        c = wgetch(help_win);

        switch (c) {
            case KEY_UP:
                highlight = (highlight == 0) ? num_commands - 1 : highlight - 1;
                break;
            case KEY_DOWN:
                highlight = (highlight == num_commands - 1) ? 0 : highlight + 1;
                break;
            case 'q':
                delwin(help_win);
                refresh(); // Refresh the main window to ensure no artifacts remain
                return;
            default:
                break;
        }
    }
}


WINDOW *create_centered_window(int height, int width) {
    int startx, starty;
    int x, y;

    // Get the screen dimensions
    getmaxyx(stdscr, y, x);

    // Calculate the starting position for the window
    startx = (x - width) / 2;
    starty = (y - height) / 2;

    // Create the window
    WINDOW *popup_win = newwin(height, width, starty, startx);

    // Initialize the window
    box(popup_win, 0, 0);
    wrefresh(popup_win);

    return popup_win;
}

void check_term_size(WINDOW* win, WINDOW* info_win) {
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int win_width = COLS / 2;
    int win_height = rows - 1;
    int win_startx = 0;
    int win_starty = 0;

    int info_width = COLS / 2;
    int info_height = rows - 1;
    int info_startx = COLS / 2;
    int info_starty = 0;

    int win_rows, win_cols;
    getmaxyx(win, win_rows, win_cols);

    // Check if the terminal size has changed
    if (win_rows!= win_height || win_cols!= win_width) {
        // Update the window sizes and positions
        wresize(win, win_height, win_width);
        mvwin(win, win_starty, win_startx);

        wresize(info_win, info_height, info_width);
        mvwin(info_win, info_starty, info_startx);

        // Redraw the borders and items
        werase(win);
        draw_colored_border(win, 2);
        wrefresh(win);

        werase(info_win);
        box(info_win, 0, 0);
        wrefresh(info_win);
    }
}
