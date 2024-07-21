// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // //

/* By nots1dd */

#include <stdio.h>     // For FILE, fopen, fgets, fclose, perror
#include <stdlib.h>    // For exit, EXIT_FAILURE
#include <ncurses.h>   // For NCurses functions and types

#define MAX_LINES 60   // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256 // Define the maximum line length

void display_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        endwin();
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int info_win_height = 10;
    int info_win_width = COLS - 4;
    int info_win_y = (LINES - info_win_height) / 2;
    int info_win_x = (COLS - info_win_width) / 2;

    WINDOW *win = newwin(info_win_height, info_win_width, info_win_y, info_win_x);
    box(win, 0, 0);  // Draw a border around the window
    mvwprintw(win, 0, 2, " File Preview ");  // Add a title to the window

    char line[MAX_LINE_LENGTH];
    int row = 1;  // Start at 1 to account for the border

    // Read file and display lines up to MAX_LINES
    while (fgets(line, MAX_LINE_LENGTH, file) && row < info_win_height - 1) {
        mvwprintw(win, row, 1, "%s", line);
        row++;
    }
    fclose(file);

    wrefresh(win);  // Refresh the window to show the content

    delwin(win);  // Delete the window when done
}

int is_readable_extension(const char *filename) {
    // List of supported extensions
    const char *supported_extensions[] = {".txt", ".json", ".conf", ".log", ".md", ".c", ".h", ".cpp", ".cxx", ".cc", "hpp", ".js", ".ts", "jsx", ".java", ".dart", ".asm", ".py", ".rust", ".go", ".perl", NULL}; // will add more ig
    const char *ext = strrchr(filename, '.');
    if (ext) {
        for (int i = 0; supported_extensions[i] != NULL; i++) {
            if (strcmp(ext, supported_extensions[i]) == 0) {
                return 1;
            }
        }
    }
    return 0;
}
