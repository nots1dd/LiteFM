// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // //

/* By nots1dd */

#include "../include/filepreview.h"
#include "../include/cursesutils.h"
#include "../include/signalhandling.h"
#include "../include/logging.h"
#include "../include/syntax.h"

#define MAX_LINES 60   // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256 // Define the maximum line length
#define MAX_FILE_HEADER_SIZE 18  // Maximum size needed for the longest signature

typedef struct {
    const char *signature;
    size_t length;
    const char *file_type;
} FileSignature;

// List of known file signatures
FileSignature file_signatures[] = {
    {"\x7F\x45\x4C\x46", 4, "ELF executable"},  // ELF executable
    {"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8, "PNG image"},  // PNG image
    {"\xFF\xD8\xFF", 3, "JPEG image"},  // JPEG image
    {"#!/bin/bash", 10, "shell script"},  // Shell script
    {"#!/usr/bin/env bash", 18, "shell script"},  // Shell script
    {NULL, 0, NULL}  // End of list marker
};

const char * get_file_extension(const char * filename) {
  const char * dot = strrchr(filename, '.');
  if (!dot || dot == filename) return "";
  return dot + 1;
}

const char *determine_file_type(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return NULL;
    }

    unsigned char header[MAX_FILE_HEADER_SIZE];
    size_t bytes_read = fread(header, 1, MAX_FILE_HEADER_SIZE, file);
    fclose(file);

    if (bytes_read == 0) {
        return NULL;
    }

    for (int i = 0; file_signatures[i].signature != NULL; i++) {
        if (bytes_read >= file_signatures[i].length &&
            memcmp(header, file_signatures[i].signature, file_signatures[i].length) == 0) {
            return file_signatures[i].file_type;
        }
    }

    return "Unknown";
}

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

    // Initialize color pairs for syntax highlighting
    start_color();
    init_pair(20, COLOR_BLUE, COLOR_BLACK); // Keywords
    init_pair(21, COLOR_GREEN, COLOR_BLACK); // Comments
    init_pair(22, COLOR_YELLOW, COLOR_BLACK); // Numbers
    init_pair(23, COLOR_CYAN, COLOR_BLACK);   // Strings
    init_pair(24, COLOR_WHITE, COLOR_BLACK);  // Default text

    // Read file and display lines with syntax highlighting
    while (fgets(line, sizeof(line), file) && row < MAX_LINES - 1) {
        // Remove newline character from the line if present
        line[strcspn(line, "\n")] = '\0';

        // Highlight syntax in the line and display
        mvwprintw(info_win, row, 1, "%s", line);
        row++;
        lines_read++;
    }

    if (lines_read == 0) { 
        int empty_message_size = sizeof(empty_message) / sizeof(empty_message[0]);
        for (int j = 0; j < empty_message_size; j++) {
            mvwprintw(info_win, j + 3, 2, "%s", empty_message[j]);
        }
        mvwprintw(info_win, empty_message_size + 4, 2, "Printed by LiteFM");
    }
    fclose(file);

    // Draw border and refresh window
    draw_colored_border(info_win, 4);
    wrefresh(info_win);  // Refresh the window to show the content
}


int is_readable_extension(const char *filename) {
    // List of supported extensions
    const char *supported_extensions[] = {
        ".txt", ".sh", ".json", ".conf", ".log", ".md", ".c", ".h", ".hpp", ".cpp", ".cxx", ".cc", ".js", 
        ".ts", ".jsx", ".java", ".dart", ".asm", ".py", ".rs", ".go", ".pl", ".backup", ".rasi", ".nix", 
        ".yaml", ".html", ".css", ".scss", NULL
    };
    
    // Check if the file has a supported extension
    const char *ext = get_file_extension(filename);
    if (ext[0] != '\0') {
        for (int i = 0; supported_extensions[i] != NULL; i++) {
            if (strcmp(ext, supported_extensions[i] + 1) == 0) {  // +1 to skip the initial dot
                return 1;
            }
        }
    }

    // Check file type based on header
    const char *file_type = determine_file_type(filename);
    if (file_type && (
        strcmp(file_type, "ASCII text") == 0 ||
        strcmp(file_type, "shell script") == 0 ||
        strcmp(file_type, "JSON data") == 0 ||
        strcmp(file_type, "UTF-8 Unicode text") == 0 ||
        strcmp(file_type, "C source") == 0 ||
        strcmp(file_type, "C++ source") == 0 ||
        strcmp(file_type, "Python script") == 0 ||
        strcmp(file_type, "Java source") == 0 ||
        strcmp(file_type, "HTML document") == 0 ||
        strcmp(file_type, "CSS document") == 0
    )) {
        return 1;
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

int is_image(const char *filename) {
    const char *image_extensions[] = {".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff", ".webp"};
    size_t num_extensions = sizeof(image_extensions) / sizeof(image_extensions[0]);

    const char *ext = strrchr(filename, '.');
    if (!ext) return 0;

    char ext_lower[16];
    size_t i;
    for (i = 0; ext[i] && i < sizeof(ext_lower) - 1; i++) {
        ext_lower[i] = tolower((unsigned char)ext[i]);
    }
    ext_lower[i] = '\0';

    for (i = 0; i < num_extensions; i++) {
        if (strcmp(ext_lower, image_extensions[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

// Function to view an image
void view_image(WINDOW *win, const char *current_path, const char *filename) {
    // Create a new NCurses window
    WINDOW *image_win = newwin(10, 40, 2, 2);
    box(image_win, 0, 0);
    mvwprintw(image_win, 0, 2, " LiteFM ");
    wrefresh(image_win);

    // Construct the full file path
    char file_path[PATH_MAX];
    snprintf(file_path, sizeof(file_path), "%s/%s", current_path, filename);

    // Execute imgcurses command in the NCurses window
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "imgcurses %s", file_path);
    def_prog_mode();
    endwin();
    int status = system(cmd);
    reset_prog_mode();

    // Check the exit code of imgcurses
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        // imgcurses was successful, refresh and wait for user input
        refresh();
        wrefresh(image_win);

        getch();
    }

    // Clean up
    delwin(image_win);
}

bool is_valid_editor(const char *editor) {
    for (size_t i = 0; i < strlen(editor); ++i) {
        if (!isalnum(editor[i]) && editor[i] != '-' && editor[i] != '_') {
            return false;
        }
    }
    return true;
}

void launch_env_var(WINDOW* win, const char *current_path, const char *filename, const char* env_var) {
    const char* editor = getenv(env_var);
    if (editor == NULL) {
        if (strcmp(env_var, "EDITOR") == 0) {
            editor = getenv("VISUAL");
        } else if (strcmp(env_var, "VISUAL") == 0) {
            editor = getenv("EDITOR");
        }
        if (editor == NULL) {
            editor = "nano"; // Default to GNU nano if neither EDITOR nor VISUAL is set
        }
    }

    log_message(LOG_LEVEL_DEBUG, "Launching `%s` in %s...", filename, editor);

    // Construct the full file path
    char file_path[PATH_MAX];
    snprintf(file_path, sizeof(file_path), "%s/%s", current_path, filename);

    // End NCurses mode before launching the editor
    endwin();

    ignore_sigwinch(); // ignore the signal sent when resizing a window

    // Fork a new process to launch the editor
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        char *args[] = {(char *)editor, file_path, NULL};
        execvp(editor, args); // Execute the editor

        // If execvp fails
        perror("Failed to launch environment variable");
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Fork failed
        log_message(LOG_LEVEL_ERROR, "Fork failed.");
        show_term_message("Error while forking process", 1);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            log_message(LOG_LEVEL_DEBUG, "%s: Exited editor successfully", filename);
        } else {
            log_message(LOG_LEVEL_ERROR, "Error while executing `%s`", editor);
            show_term_message("Error while calling editor", 1);
        }
    }

    // Restore default SIGWINCH handler after the editor exits
    restore_sigwinch();

    // Reinitialize NCurses mode after the editor exits
    initscr();
    cbreak();
    noecho();
    keypad(win, TRUE);
    refresh();
    clear();
}

