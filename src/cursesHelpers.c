#include <ncurses.h>

void show_message(WINDOW *win, const char *message) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    mvwprintw(win, maxy - 2, 2, message);
    wrefresh(win);
}

void clearLine(WINDOW *win, int x, int y) {
  mvwprintw(win, x, y, "\n");
}

void show_term_message(const char *message) { 
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    int message_y = maxy - 1;  // Adjust position as needed
    move(message_y, 0);
    clrtoeol();

    move(message_y, 0);
    clrtoeol();
    refresh();

    mvprintw(message_y, 0, "%s", message);
    refresh();
    
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

void get_user_input_from_bottom(WINDOW *win, char *buffer, int max_length, const char* type) {
    echo();
    nodelay(win, FALSE);
    attron(A_BOLD);  // Turn on bold attribute
    if (type == "search") {
      mvprintw(LINES - 1, 0, " /");
    }
    attroff(A_BOLD);  // Turn off bold attribute
    clrtoeol();  // Clear the rest of the line to handle previous content
    wgetnstr(win, buffer, max_length);
    noecho();
    nodelay(win, TRUE);
    mvprintw(LINES - 1, 0, " ");  // Clear the prompt after getting input
    clrtoeol();  // Clear the rest of the line to handle previous content
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
