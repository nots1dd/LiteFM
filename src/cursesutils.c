// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include <ncurses.h>
#include <locale.h>

void show_message(WINDOW *win, const char *message) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    mvwprintw(win, maxy - 2, 2, message);
    wrefresh(win);
}

void clearLine(WINDOW *win, int x, int y) {
  mvwprintw(win, x, y, "\n");
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
    
    if (err!=0) {
      attron(COLOR_PAIR(12));
      mvprintw(message_y, 0, " \u2718 %s", message); // \u2718 is unicode for ✘
      attroff(COLOR_PAIR(12));
    } else {
      attron(COLOR_PAIR(1));
      mvprintw(message_y, 0, " \u2714 %s", message); // \u2714 is unicode for ✔
      attroff(COLOR_PAIR(1));
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
    init_pair(12, COLOR_RED, COLOR_BLACK);
}

int confirm_action(WINDOW *win, const char *message) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);

    WINDOW *confirm_win = newwin(5, maxx - 4, maxy / 2 - 2, 2);
    box(confirm_win, 0, 0);
    mvwprintw(confirm_win, 1, 2, message);
    mvwprintw(confirm_win, 3, 2, "Press 'y' to confirm, 'n' to cancel.");
    wrefresh(confirm_win);

    int ch = wgetch(confirm_win);
    delwin(confirm_win);

    return ch == 'y' || ch == 'Y' || ch == 10;
}

void draw_3d_info_win(WINDOW *win, int y, int x, int height, int width, int color_pair, int shadow_color_pair) {
    // Create shadow window
    WINDOW *shadow_win = newwin(height, width, y + 1, x + 2);
    wattron(shadow_win, COLOR_PAIR(shadow_color_pair)); 
    wattroff(shadow_win, COLOR_PAIR(shadow_color_pair));
    box(shadow_win, 0, 0);
    wrefresh(shadow_win);

    
}

void get_user_input_from_bottom(WINDOW *win, char *buffer, int max_length, const char* type) {
    echo();
    nodelay(win, FALSE);
    attron(A_BOLD);  // Turn on bold attribute
    if (type == "search") {
      attron(COLOR_PAIR(3));
      mvprintw(LINES - 1, 0, " /");
      attroff(COLOR_PAIR(3));
    } else if (type == "rename") {
      attron(COLOR_PAIR(2));
      mvprintw(LINES - 1, 0, " Rename: ");
      attroff(COLOR_PAIR(2));
    }
    attroff(A_BOLD);  // Turn off bold attribute
    clrtoeol();  // Clear the rest of the line to handle previous content
    attron(COLOR_PAIR(3));
    wgetnstr(win, buffer, max_length);
    noecho();
    nodelay(win, TRUE);
    mvprintw(LINES - 1, 0, " ");  // Clear the prompt after getting input
    clrtoeol();  // Clear the rest of the line to handle previous content
    attroff(COLOR_PAIR(3));
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


