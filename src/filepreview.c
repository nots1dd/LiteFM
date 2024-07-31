// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // //

/* By nots1dd */

#include "../filepreview.h"
#include "../cursesutils.h"

#define MAX_LINES 60   // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256 // Define the maximum line length

const char *empty_message[] = {
    " _____ __  __ ____ _______   __  _____ ___ _     _____   _ ",
    "| ____|  \\/  |  _ \\_   _\\ \\ / / |  ___|_ _| |   | ____| | |",
    "|  _| | |\\/| | |_) || |  \\ V /  | |_   | || |   |  _|   | |",
    "| |___| |  | |  __/ | |   | |   |  _|  | || |___| |___  |_/",
    "|_____|_|  |_|_|    |_|   |_|   |_|   |___|_____|_____| (_)",
    " "
};

void display_file(WINDOW *info_win, const char *filename) {
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
    int row = 3;  // Start at row 3 to account for the title and spacing
    int lines_read = 0;
    char sanitizedCurPath[PATH_MAX];
    if (strncmp(filename, "//", 2) == 0) {
      snprintf(sanitizedCurPath, sizeof(sanitizedCurPath), "%s", filename + 1);
    } else {
    strcpy(sanitizedCurPath, filename);
  }
    wattron(info_win, A_BOLD | COLOR_PAIR(9));
    mvwprintw(info_win, 1, 1, " %s ", sanitizedCurPath);
    wattroff(info_win, A_BOLD | COLOR_PAIR(9));

    // Read file and display lines up to MAX_LINES
    wattron(info_win, COLOR_PAIR(7));
    while (fgets(line, sizeof(line), file) && row < MAX_LINES - 1) {
        // Remove newline character from the line if present
        line[strcspn(line, "\n")] = '\0';
        mvwprintw(info_win, row, 1, "%s", line);
        row++;
        lines_read++;
    }
    
    if (lines_read == 0) {
        // If no lines were read, display "Null File."
        int empty_message_size = sizeof(empty_message) / sizeof(empty_message[0]);
        for (int j=0;j<empty_message_size;j++) {
          mvwprintw(info_win, j+3, 2, "%s", empty_message[j]);
        }
        mvwprintw(info_win, empty_message_size+4, 2, "Printed by LiteFM");
    }

    fclose(file);
    wattroff(info_win, COLOR_PAIR(7));

    draw_colored_border(info_win, 4);
    wrefresh(info_win);  // Refresh the window to show the content
}

int is_readable_extension(const char *filename) {
    // List of supported extensions
    const char *supported_extensions[] = {".txt", ".sh", ".json", ".conf", ".log", ".md", ".c", ".h", ".hpp", ".cpp", ".cxx", ".cc", "hpp", ".js", ".ts", "jsx", ".java", ".dart", ".asm", ".py", ".rust", ".go", ".perl", ".backup", ".rasi", ".nix", ".yaml", ".html", ".css", ".scss", NULL}; // will add more ig
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

const char *format_file_size(off_t size) {
    static char formatted_size[64];

    // Check if the size is at least 1 byte
    if (size >= 1ULL << 30) {  // GiB
        snprintf(formatted_size, sizeof(formatted_size), "%.2f GiB", (double)size / (1ULL << 30));
    } else if (size >= 1ULL << 20) {  // MiB
        snprintf(formatted_size, sizeof(formatted_size), "%.2f MiB", (double)size / (1ULL << 20));
    } else if (size >= 1ULL << 10) {  // KiB
        snprintf(formatted_size, sizeof(formatted_size), "%.2f KiB", (double)size / (1ULL << 10));
    } else {  // Bytes
        snprintf(formatted_size, sizeof(formatted_size), "%ld bytes", size);
    }
    return formatted_size;
}

