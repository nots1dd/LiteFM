#include <ncurses.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <pwd.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <time.h>
#include <archive.h>
#include <archive_entry.h>
#include "src/getFreeSpace.c"
// #include "src/filePreview.c"

#define MAX_ITEMS 1024
#define MAX_HISTORY 256

typedef struct {
    char name[NAME_MAX];
    int is_dir;
} FileItem;

typedef struct {
    char path[PATH_MAX];
    int highlight;
} DirHistory;

// function prototypes
void show_term_message(const char *message);

void list_dir(const char *path, FileItem items[], int *count, int show_hidden) {
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(path)))
        return;

    *count = 0;
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files unless show_hidden is set, also ignore '.' and '..'
        if ((!show_hidden && entry->d_name[0] == '.') || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        strcpy(items[*count].name, entry->d_name);
        items[*count].is_dir = entry->d_type == DT_DIR;
        (*count)++;
    }
    closedir(dir);
}

void print_items(WINDOW *win, FileItem items[], int count, int highlight, const char *current_path, int show_hidden, int scroll_position, int height) {
    char *hidden_dir;
    if (show_hidden) {
        hidden_dir = "ON";
    } else {
        hidden_dir = "OFF";
    }
    // getting system free space from / dir 
    double systemFreeSpace = system_free_space("/");

    // Define color pairs
    init_pair(1, COLOR_GREEN, COLOR_BLACK);  // File color (Green)
    init_pair(2, COLOR_BLUE, COLOR_BLACK);   // Directory color (Blue)
    init_pair(3, COLOR_RED, COLOR_BLACK);

    // Print title
    wattron(win, A_BOLD | A_UNDERLINE);
    mvwprintw(win, 1, 2, " LITE FM ");
    wattroff(win, A_BOLD | A_UNDERLINE);

    // Print current path and hidden directories status
    wattron(win, A_BOLD);
    mvwprintw(win, 3, 2, "Browsing: %s ", current_path);
    mvwprintw(win, LINES - 3, COLS - 35, "Hidden Dirs: %s", hidden_dir);
    mvwprintw(win, LINES - 3, COLS - 15, "%.2f GiB", systemFreeSpace);
    wattroff(win, A_BOLD);

    // Print items
    if (count == 0) {
        wattron(win, COLOR_PAIR(3));
        mvwprintw(win, 7, 2, "No files or directories.");
        wattroff(win, COLOR_PAIR(3));
    } else {
        for (int i = 0; i < height - 7 && i + scroll_position < count; i++) {
            int index = i + scroll_position;
            if (index == highlight)
                wattron(win, A_REVERSE);

            // Apply color based on file type
            if (items[index].is_dir)
                wattron(win, COLOR_PAIR(2));
            else
                wattron(win, COLOR_PAIR(1));
                wattron(win, A_BOLD);

            mvwprintw(win, i + 5, 4, " %s ", items[index].name);

            // Turn off color attributes
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(1));
            wattroff(win, COLOR_PAIR(2));

            if (index == highlight)
                wattroff(win, A_REVERSE);
        }
    }
}

void show_message(WINDOW *win, const char *message) {
    int maxy, maxx;
    getmaxyx(win, maxy, maxx);
    mvwprintw(win, maxy - 2, 2, message);
    wrefresh(win);
}

void get_home_directory(char *home_path) {
    struct passwd *pw = getpwuid(getuid());
    strcpy(home_path, pw->pw_dir);
}

int create_file(const char *path, const char *filename, char *timestamp) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, filename); 

    // Create the file
    int fd = open(full_path, O_CREAT | O_EXCL | O_WRONLY, 0666);
    if (fd == -1) {
        if (errno == EEXIST)
            return 1; // File already exists
        else
            return -1; // Error creating file
    }

    close(fd);

    // Get the current time and format it
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    return 0; // File created successfully
}

long get_file_size(const char *file_path) {
    struct stat st;
    if (stat(file_path, &st) == 0) {
        return st.st_size;
    }
    return -1;
}

// Function to extract an archive specified by archive_path
int extract_archive(const char *archive_path) {
    char command[PATH_MAX + 100]; // +100 for safety margin
    int result;
    long file_size_before, file_size_after;

    // Initialize NCurses
    initscr(); // Start NCurses mode
    WINDOW *progress_win = newwin(3, 70, LINES / 2, (COLS - 70) / 2); // Create a new window
    box(progress_win, 0, 0); // Add a border to the window
    mvwprintw(progress_win, 1, 1, "Extraction Progress: [>%-69s]", ""); // Initial progress bar setup
    wrefresh(progress_win); // Refresh the window to show changes

    // Get the directory of the archive (example.zip)
    char archive_dir[PATH_MAX];
    strncpy(archive_dir, archive_path, PATH_MAX);
    dirname(archive_dir); // Get the directory part of the archive path

    // Create the extraction directory (filename_extracted)
    char *archive_basename = basename(archive_path);
    char extraction_dir[PATH_MAX];
    snprintf(extraction_dir, sizeof(extraction_dir), "%s/%s_extracted", archive_dir, archive_basename);

    // Determine archive type and construct extraction command
    if (strstr(archive_path, ".zip")) {
        snprintf(command, sizeof(command), "mkdir -p \"%s\" && unzip -o \"%s\" -d \"%s\" > /dev/null 2>&1", extraction_dir, archive_path, extraction_dir);
    } else if (strstr(archive_path, ".tar")) {
        snprintf(command, sizeof(command), "mkdir -p \"%s\" && tar -xf \"%s\" -C \"%s\" > /dev/null 2>&1", extraction_dir, archive_path, extraction_dir);
    } else {
        endwin(); // End NCurses mode before returning
        return -1; // Unsupported archive format
    }

    // Get file size before extraction
    file_size_before = get_file_size(archive_path);

    // Execute command
    FILE *extract_process = popen(command, "r");
    if (extract_process == NULL) {
        endwin(); // End NCurses mode before returning
        return -1; // Failed to execute command
    }

    // Monitor extraction progress
    while (fgets(command, sizeof(command), extract_process) != NULL) {
        // You can parse command output here if needed
        // For simplicity, we'll just update the progress bar based on file size changes
        file_size_after = get_file_size(archive_path);
        if (file_size_before != -1 && file_size_after != -1) {
            float progress = ((float)file_size_after) / file_size_before;
            int pos = (int)(69 * progress); // Adjust 69 for the width of the progress bar

            // Update progress bar in NCurses window
            mvwprintw(progress_win, 1, 24, "[%.*s>%*s]", pos, "=======================================================================", 69 - pos, "");
            wrefresh(progress_win); // Refresh the window to show changes
        }
    }

    pclose(extract_process); // Close the process

    // End NCurses mode
    endwin();

    return 0; // Extraction successful
}


int create_directory(const char *path, const char *dirname, char *timestamp) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, dirname);

    // Create the directory
    if (mkdir(full_path, 0777) == -1) {
        if (errno == EEXIST)
            return 1; // Directory already exists
        else
            return -1; // Error creating directory
    }

    // Get the current time and format it
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(timestamp, 26, "%Y-%m-%d %H:%M:%S", tm_info);

    return 0; // Directory created successfully
}

int remove_file(const char *path, const char *filename) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, filename);

    // Remove the file
    if (unlink(full_path) == -1)
        return -1; // Error removing file

    return 0; // File removed successfully
}

/* NOT RECURSIVE */
int remove_directory(const char *path, const char *dirname) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, dirname);

    // Remove the directory and its contents
    if (rmdir(full_path) == -1)
        return -1; // Error removing directory

    return 0; // Directory removed successfully
}

/* probably the most dangerous function (it works pretty well tho) */
int remove_directory_recursive(const char *base_path, const char *dirname) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", base_path, dirname);

    DIR *dir = opendir(full_path);
    if (dir == NULL) {
        show_term_message("Error opening directory");
        return -1;
    }

    struct dirent *entry;
    int num_files_deleted = 0;
    int num_dirs_deleted = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char file_path[PATH_MAX];
        snprintf(file_path, PATH_MAX, "%s/%s", full_path, entry->d_name);

        struct stat statbuf;
        if (stat(file_path, &statbuf) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                // Recursively remove subdirectory
                if (remove_directory_recursive(full_path, entry->d_name) != 0) {
                    closedir(dir);
                    return -1;
                }
                num_dirs_deleted++;
                char message[PATH_MAX];
                snprintf(message, PATH_MAX, "Deleted directory: %s", file_path);
                show_term_message(message);
            } else {
                // Remove file
                if (unlink(file_path) != 0) {
                    char message[PATH_MAX];
                    snprintf(message, PATH_MAX, "Error removing file: %s", file_path);
                    show_term_message(message);
                    closedir(dir);
                    return -1;
                }
                num_files_deleted++;
                char message[PATH_MAX];
                snprintf(message, PATH_MAX, "Deleted file: %s", file_path);
                show_term_message(message);
            }
        } else {
            char message[PATH_MAX];
            snprintf(message, PATH_MAX, "Error getting status of file: %s", file_path);
            show_term_message(message);
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);

    // Remove the now-empty directory
    if (rmdir(full_path) != 0) {
        char message[PATH_MAX];
        snprintf(message, PATH_MAX, "Error removing directory: %s", full_path);
        show_term_message(message);
        return -1;
    }
    num_dirs_deleted++;
    char message[PATH_MAX];
    snprintf(message, PATH_MAX, "Deleted directory: %s", full_path);
    show_term_message(message);

    char message2[PATH_MAX];
    snprintf(message2, PATH_MAX, "Deleted %d files and %d directories", num_files_deleted, num_dirs_deleted);
    show_term_message(message2);

    return 0;
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

    return ch == 'y' || ch == 'Y';
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

int find_item(const char *query, FileItem items[], int item_count, int *start_index) {
    char lower_query[NAME_MAX];
    for (int i = 0; query[i] && i < NAME_MAX; i++) {
        lower_query[i] = tolower(query[i]);
    }
    lower_query[strlen(query)] = '\0';

    for (int i = *start_index; i < item_count; i++) {
        char lower_name[NAME_MAX];
        for (int j = 0; items[i].name[j] && j < NAME_MAX; j++) {
            lower_name[j] = tolower(items[i].name[j]);
        }
        lower_name[strlen(items[i].name)] = '\0';

        if (strstr(lower_name, lower_query) != NULL) {
            *start_index = i;
            return i;
        }
    }
    for (int i = 0; i < *start_index; i++) {
        char lower_name[NAME_MAX];
        for (int j = 0; items[i].name[j] && j < NAME_MAX; j++) {
            lower_name[j] = tolower(items[i].name[j]);
        }
        lower_name[strlen(items[i].name)] = '\0';

        if (strstr(lower_name, lower_query) != NULL) {
            *start_index = i;
            return i;
        }
    }
    return -1; // Not found
}

void get_file_info(WINDOW *main_win, const char *path, const char *filename) {
    struct stat file_stat;
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, filename);

    // Get file information
    if (stat(full_path, &file_stat) == -1) {
        show_message(main_win, "Error retrieving file information.");
        return;
    }

    // Create a new window for displaying file information
    int info_win_height = 10;
    int info_win_width = COLS - 4;
    int info_win_y = (LINES - info_win_height) / 2;
    int info_win_x = (COLS - info_win_width) / 2;

    WINDOW *info_win = newwin(info_win_height, info_win_width, info_win_y, info_win_x);
    box(info_win, 0, 0);

    // Display file information
    mvwprintw(info_win, 1, 2, "File Information:");
    mvwprintw(info_win, 3, 2, "Name: %s", filename);
    mvwprintw(info_win, 4, 2, "Size: %ld bytes", file_stat.st_size);

    const char *file_ext = strrchr(filename, '.');
    if (file_ext != NULL) {
        mvwprintw(info_win, 6, 2, "Extension: %s", file_ext + 1);
    } else {
        mvwprintw(info_win, 6, 2, "Extension: none");
    }

    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
    mvwprintw(info_win, 7, 2, "Last Modified: %s", mod_time);
    
    mvwprintw(info_win, 1, 20, (S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    mvwprintw(info_win, 1, 21, (file_stat.st_mode & S_IRUSR) ? "r" : "-");
    mvwprintw(info_win, 1, 22, (file_stat.st_mode & S_IWUSR) ? "w" : "-");
    mvwprintw(info_win, 1, 23, (file_stat.st_mode & S_IXUSR) ? "x" : "-");
    mvwprintw(info_win, 1, 24, (file_stat.st_mode & S_IRGRP) ? "r" : "-");
    mvwprintw(info_win, 1, 25, (file_stat.st_mode & S_IWGRP) ? "w" : "-");
    mvwprintw(info_win, 1, 26, (file_stat.st_mode & S_IXGRP) ? "x" : "-");
    mvwprintw(info_win, 1, 27, (file_stat.st_mode & S_IROTH) ? "r" : "-");
    mvwprintw(info_win, 1, 28, (file_stat.st_mode & S_IWOTH) ? "w" : "-");
    mvwprintw(info_win, 1, 29, (file_stat.st_mode & S_IXOTH) ? "x" : "-");

    // Additional file attributes can be displayed here

    mvwprintw(info_win, info_win_height - 2, 2, "Press any key to close this window.");
    wrefresh(info_win);

    // Wait for user input to close the window
    wgetch(info_win);

    // Clean up: delete the window
    delwin(info_win);

    // Refresh the main window to ensure no artifacts remain
    werase(main_win);
    box(main_win, 0, 0);
    wrefresh(main_win);
}



int main() {
    initscr();
    start_color();  // Enable color
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(0);

    int highlight = 0;
    FileItem items[MAX_ITEMS];
    int item_count = 0;
    char current_path[PATH_MAX];
    DirHistory history[MAX_HISTORY];
    int history_count = 0;
    int show_hidden = 0;  // Flag to toggle showing hidden files
    int scroll_position = 0;  // Position of the first visible item

    get_home_directory(current_path);
    list_dir(current_path, items, &item_count, show_hidden);

    // Create a new window with a border
    int startx = 0, starty = 0;
    int width = COLS - 1, height = LINES - 1;
    WINDOW *win = newwin(height, width, starty, startx);
    box(win, 0, 0);

    // Initial display
    print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
    wrefresh(win);

    timeout(1);
    nodelay(win, TRUE);

    int find_index = 0;
    char last_query[NAME_MAX] = "";
    bool firstKeyPress = true;

    while (true) {
        int choice = getch();
        if (firstKeyPress) {
            werase(win);
            box(win, 0, 0);
            print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
            wrefresh(win);
       }
        firstKeyPress = false;
        if (choice != ERR) {
            switch (choice) {
                case KEY_UP:
                case 'k':
                    if (highlight > 0) {
                        highlight--;
                        if (highlight < scroll_position) {
                            scroll_position--;
                        }
                    }
                    break;
                case KEY_DOWN:
                case 'j':
                    if (highlight < item_count - 1) {
                        highlight++;
                        if (highlight >= scroll_position + height - 8) {
                            scroll_position++;
                        }
                    }
                    break;
                case KEY_LEFT:
                case 'h':
                    if (history_count > 0) {
                        history_count--;
                        strcpy(current_path, history[history_count].path);
                        highlight = history[history_count].highlight;
                        list_dir(current_path, items, &item_count, show_hidden);
                        scroll_position = 0;
                    }
                    break;
                case KEY_RIGHT:
                case 'l':
                    if (items[highlight].is_dir) {
                        if (history_count < MAX_HISTORY) {
                            strcpy(history[history_count].path, current_path);
                            history[history_count].highlight = highlight;
                            history_count++;
                        }
                        strcat(current_path, "/");
                        strcat(current_path, items[highlight].name);
                        list_dir(current_path, items, &item_count, show_hidden);
                        highlight = 0;
                        scroll_position = 0;
                    }
                    break;
                case '.':
                    show_hidden = !show_hidden;  // Toggle show_hidden flag
                    list_dir(current_path, items, &item_count, show_hidden);
                    highlight = 0;
                    scroll_position = 0;
                    break;
                
                case 'a': // Add file or directory
                    {
                        WINDOW *input_win = newwin(3, COLS - 150, LINES - 5, 2);
                        box(input_win, 0, 0);
                        mvwprintw(input_win, 0, 1, " %s/ ", current_path);
                        wrefresh(input_win);

                        char name_input[NAME_MAX];
                        get_user_input(input_win, name_input, NAME_MAX);

                        delwin(input_win);

                        if (strlen(name_input) > 0) {
                            char timestamp[26];
                            if (name_input[strlen(name_input) - 1] == '/') {
                                // Create directory
                                name_input[strlen(name_input) - 1] = '\0'; // Remove trailing slash
                                int result = create_directory(current_path, name_input, timestamp);
                                if (result == 0) {
                                    char msg[256];
                                    snprintf(msg, sizeof(msg), "Directory '%s' created at %s.", name_input, timestamp);
                                    show_term_message(msg);
                                } else if (result == 1) {
                                    show_term_message("Directory already exists.");
                                } else {
                                    show_term_message("Error creating directory.");
                                }
                            } else {
                                // Create file
                                int result = create_file(current_path, name_input, timestamp);
                                if (result == 0) {
                                    char msg[256];
                                    snprintf(msg, sizeof(msg), "File '%s' created at %s.", name_input, timestamp);
                                    show_term_message(msg);
                                } else if (result == 1) {
                                    show_term_message("File already exists.");
                                } else {
                                    show_term_message("Error creating file.");
                                }
                            }
                            list_dir(current_path, items, &item_count, show_hidden);
                            scroll_position = 0;
                        }
                    }
                    break;

                case 'd': // Remove file or directory
                    {
                        if (item_count > 0) {
                            char confirm_msg[256];
                            if (items[highlight].is_dir) {
                                snprintf(confirm_msg, sizeof(confirm_msg), "Remove Directory '%s'? (y/n)", items[highlight].name);
                            } else {
                                snprintf(confirm_msg, sizeof(confirm_msg), "Remove file '%s'? (y/n)", items[highlight].name);
                            }

                            if (confirm_action(win, confirm_msg)) {
                                char *deldir = items[highlight].name;
                                if (items[highlight].is_dir) {
                                    int result = remove_directory(current_path, items[highlight].name);
                                    if (result != 0) {            
                                        show_term_message("Error removing directory.");
                                    } else {
                                        char delmsg[256];
                                        snprintf(delmsg, sizeof(delmsg), "Directory '%s' deleted", deldir);
                                        show_term_message(delmsg);
                                     } 
                                } else {
                                    char *delfile = items[highlight].name;
                                    int result = remove_file(current_path, items[highlight].name);
                                    if (result != 0) {        
                                      show_term_message("Error removing file.");
                                    } else {
                                        char msg[256];
                                    snprintf(msg, sizeof(msg), "File '%s' deleted", delfile);
                                    show_term_message(msg);
                                   }
                                }
                                list_dir(current_path, items, &item_count, show_hidden);
                                highlight = 0; 
                                scroll_position = 0;
                            }
                        }
                    }
                    break;
               
               case 'D':
                  {
                    if (item_count > 0) {
                            char confirm_msg[256];
                            if (items[highlight].is_dir) {
                                snprintf(confirm_msg, sizeof(confirm_msg), "[DANGER] Remove Directory recursively '%s'? (y/n)", items[highlight].name);
                            } else {
                                snprintf(confirm_msg, sizeof(confirm_msg), "This command is for deleting dirs recursively only!");
                            }

                            if (confirm_action(win, confirm_msg)) {
                                char *deldir = items[highlight].name;
                                if (items[highlight].is_dir) {
                                    int result = remove_directory_recursive(current_path, items[highlight].name);
                                    if (result != 0) {            
                                        show_term_message("Error removing directory.");
                                    } else {
                                        char delmsg[256];
                                        snprintf(delmsg, sizeof(delmsg), "Directory '%s' deleted", deldir);
                                        show_term_message(delmsg);
                                     } 
                                } 
                                list_dir(current_path, items, &item_count, show_hidden);
                                highlight = 0; 
                                scroll_position = 0;
                            }
                        }
                  }
                  break;

                case '/': // Find file or directory
                    {   
                        wattron(win, A_BOLD);
                        mvwprintw(win, LINES - 3, COLS - 50, "Search ON");
                        wattroff(win, A_BOLD);
                        wrefresh(win);
                        char query[NAME_MAX]; 
                        get_user_input_from_bottom(stdscr, query, NAME_MAX, "search");

                        if (strlen(query) > 0) {
                            int start_index = highlight + 1;
                            int found_index = find_item(query, items, item_count, &start_index);
                            if (found_index != -1) {
                                highlight = found_index;
                                if (highlight >= scroll_position + height - 8) {
                                    scroll_position = highlight - height + 8;
                                } else if (highlight < scroll_position) {
                                    scroll_position = highlight;
                                }
                                strncpy(last_query, query, NAME_MAX);
                            } else {
                                show_term_message("Item not found.");
                            }
                        }
                    }
                    break;
                
                case 'n':
                  if (strlen(last_query) > 0) {
                        int start_index = highlight + 1;
                        int found_index = find_item(last_query, items, item_count, &start_index);
                        if (found_index != -1) {
                            highlight = found_index;
                            if (highlight >= scroll_position + height - 8) {
                                scroll_position = highlight - height + 8;
                            } else if (highlight < scroll_position) {
                                scroll_position = highlight;
                            }
                        } else {
                            show_term_message("No more occurrences found.");
                        }
                    }
                    break; 

               case 10: // Enter key
                    if (!items[highlight].is_dir) {
                        get_file_info(win, current_path, items[highlight].name);
                    }
                    break;
               
               case 'E':
                if (!items[highlight].is_dir) {
                    // Check if it's an archive file
                    const char *filename = items[highlight].name;
                    if (strstr(filename, ".zip") || strstr(filename, ".tar")) {
                        char full_path[PATH_MAX];
                        snprintf(full_path, PATH_MAX, "%s/%s", current_path, filename);

                        // Confirm extraction
                        if (confirm_action(win, "Extract this archive? (y/n)")) {
                            // Extract archive
                            if (extract_archive(full_path) == 0) {
                                show_term_message("Extraction successful.");
                                // Update file list after extraction
                                list_dir(current_path, items, &item_count, show_hidden);
                                scroll_position = 0;
                            } else {
                                show_term_message("Extraction failed.");
                            }
                        }
                    } else {
                        show_term_message("Not a supported archive format.");
                    }
                } else {
                  show_term_message("Cannot extract a directory.");
                }
                break;

                case 'q':
                    endwin();
                    return 0;
            }

            // Update display after each key press
            werase(win);
            box(win, 0, 0);
            print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
            
            wrefresh(win);
        }
    }

    endwin();
    return 0;
}
