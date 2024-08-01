// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include "../cursesutils.h"

void show_message(WINDOW *win, const char *message) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    mvwprintw(win, maxy - 2, 2, message);
    wrefresh(win);
}

void clearLine(WINDOW *win, int x, int y) {
  mvwprintw(win, x, y, "\n");
}

void colorLine(WINDOW* win, const char* info, int colorpair, int x, int y) {
  wattron(win, COLOR_PAIR(colorpair));
  mvwprintw(win, x, y, info);
  wattroff(win, COLOR_PAIR(colorpair));
} 

int show_compression_options(WINDOW *parent_win) {
    WINDOW *options_win;
    int choice;
    // Create a new window for displaying options
    int win_height = 10;
    int win_width = 40;
    int win_y = (LINES - win_height) / 2;
    int win_x = (COLS - win_width) / 2;
    
    options_win = newwin(win_height, win_width, win_y, win_x);
    box(options_win, 0, 0);
    keypad(options_win, TRUE); // Enable special keys (e.g., arrow keys)

    // Display options with color
    wattron(options_win, COLOR_PAIR(1));
    mvwprintw(options_win, 1, 2, "Select compression format:");
    wattroff(options_win, COLOR_PAIR(1));

    wattron(options_win, COLOR_PAIR(2));
    mvwprintw(options_win, 2, 2, "1. TAR (.tar)");
    mvwprintw(options_win, 3, 2, "2. ZIP (.zip)");
    wattroff(options_win, COLOR_PAIR(2));

    wattron(options_win, COLOR_PAIR(3));
    mvwprintw(options_win, 5, 2, "Press '1' for TAR, '2' for ZIP");
    mvwprintw(options_win, win_height - 2, 2, "Press any other key to exit!");
    wattroff(options_win, COLOR_PAIR(3));

    // Refresh and wait for user input
    wrefresh(options_win);
    choice = wgetch(options_win);

    // Clean up
    delwin(options_win);
    refresh(); // Refresh the main window to ensure no artifacts remain

    return (choice == '1') ? 1 : (choice == '2') ? 2 : -1;
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

void get_user_input_from_bottom(WINDOW *win, char *buffer, int max_length, const char* type) {
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
      mvprintw(y-1,0," Go to: ");
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
      wmove(win, getmaxy(win) - 1, 8);
    }
    else {
      wmove(win, getmaxy(win) - 1, 1);
    }
    attron(COLOR_PAIR(3));
    wgetnstr(win, buffer, max_length);
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
    int help_win_y = (LINES - help_win_height) / 2;
    int help_win_x = (COLS - help_win_width) / 2;
    WINDOW* help_win = newwin(help_win_height, help_win_width, help_win_y, help_win_x);
    draw_colored_border(help_win, 4);
    draw_3d_info_win(help_win, help_win_y, help_win_x, help_win_height, help_win_width, 1, 2);
    colorLine(help_win, " Help Win: ", 14, 0, 2);

    colorLine(help_win, " Scroll up            - [k/UP_ARROW]", 2, 2, 2);
    colorLine(help_win, " Scroll down          - [j/DOWN_ARROW]", 2, 3, 2);
    colorLine(help_win, " Go inside sel. dir   - [l/RIGHT_ARROW/ENTER]", 2, 4, 2);
    colorLine(help_win, " Go to parent dir     - [h/LEFT_ARROW]", 2, 5, 2);
    colorLine(help_win, " String search        - [/]", 2, 6, 2);
    colorLine(help_win, " String next match    - [n]", 2, 7, 2);
    colorLine(help_win, " String prev match    - [N]", 2, 8, 2);
    colorLine(help_win, " Go to top of list    - [gg]", 2, 9, 2);
    colorLine(help_win, " Go to end of list    - [G]", 2, 10, 2);
    colorLine(help_win, " Yank file name       - [y]", 2, 11, 2);
    colorLine(help_win, " Yank file location   - [Y]", 2, 12, 2);

    colorLine(help_win, " --------- FILE CMDS --------", 3, 14, 2);  // Using color pair 3 for section header

    colorLine(help_win, " Add a new file/dir   - [a]", 2, 16, 2);
    colorLine(help_win, " Delete file/dir      - [d] {NON-RECURSIVE DIR DELETE!}", 2, 17, 2);
    colorLine(help_win, " Recursive dir delete - [D]", 2, 18, 2);
    colorLine(help_win, " Rename a file/dir    - [R]", 2, 19, 2);
    colorLine(help_win, " Extract archive      - [E] {Works for any archive}", 2, 20, 2);
    colorLine(help_win, " Compress directory   - [Z] {Works for .zip and .tar ONLY!}", 2, 21, 2);
    colorLine(help_win, " Move a file/dir      - [M]", 2, 22, 2);

    colorLine(help_win, " Show help win        - [?]", 2, 23, 2);
    colorLine(help_win, " Go to / directory    - [H]", 2, 24, 2);
    colorLine(help_win, " Go to ~ (home) dir   - [gh]",2, 25,2);
    colorLine(help_win, " Go to input dir      - [gt]",2, 26,2);

    wrefresh(help_win);
    wgetch(help_win);
    delwin(help_win);

    // Refresh the main window to avoid some flickering issues
    wrefresh(main_win); 
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


