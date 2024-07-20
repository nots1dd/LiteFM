// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // //

/* By nots1dd */

#include <ncurses.h>
#include <errno.h>

void display_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        endwin();
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int start_row = 20;
    int start_col = 0;
    int win_height = MAX_LINES;
    int win_width = COLS - 5;

    // Create a new window
    WINDOW *win = newwin(win_height, win_width, start_row, start_col);
    box(win, 0, 0);  // Draw a border around the window
    mvwprintw(win, 0, 2, " File Preview ");  // Add a title to the window

    char line[MAX_LINE_LENGTH];
    int row = 1;  // Start at 1 to account for the border

    // Read file and display lines up to MAX_LINES
    while (fgets(line, MAX_LINE_LENGTH, file) && row < win_height - 1) {
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
