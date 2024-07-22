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

void display_file(WINDOW* info_win, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        werase(info_win);  // Clear the window first
        mvwprintw(info_win, 1, 2, "Error opening file");
        box(info_win, 0, 0);
        wrefresh(info_win);  // Refresh the window to show the error message
        return;
    }

    werase(info_win);  // Clear the window before displaying content
    mvwprintw(info_win, 0, 2, " File Preview: ");  // Add a title to the window

    char line[MAX_LINE_LENGTH];
    int row = 1;  // Start at 1 to account for the title

    // Read file and display lines up to MAX_LINES
    while (fgets(line, sizeof(line), file) && row < MAX_LINES - 1) {
        // Remove newline character from the line if present
        line[strcspn(line, "\n")] = '\0';
        mvwprintw(info_win, row, 1, "%s", line);
        row++;
    }
    if (row <= 1) {
    mvwprintw(info_win, row, 1, "Null File.");
  }
    fclose(file);

    box(info_win, 0, 0);
    wrefresh(info_win);  // Refresh the window to show the content
}


int is_readable_extension(const char *filename) {
    // List of supported extensions
    const char *supported_extensions[] = {".txt", ".sh", ".json", ".conf", ".log", ".md", ".c", ".h", ".cpp", ".cxx", ".cc", "hpp", ".js", ".ts", "jsx", ".java", ".dart", ".asm", ".py", ".rust", ".go", ".perl", ".backup", ".rasi", ".nix", ".yaml", ".html", ".css", ".scss", NULL}; // will add more ig
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
