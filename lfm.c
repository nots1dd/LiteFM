// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

/* Main C file FOR LITE FILE MANAGER */

/* LICENSED UNDER GNU GPL v3 */

#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <libgen.h>
#include <grp.h>
#include "src/getFreeSpace.c"
#include "src/cursesutils.c"
#include "src/filePreview.c"
#include "src/dircontrol.c"
#include "src/archivecontrol.c"
#include "src/logging.c"

#define MAX_ITEMS 1024
#define MAX_HISTORY 256


const char *err_message[] = {
    " _   _  ___    _____ ___ _     _____ ____    __  ____ ___ ____  ____      ",
    "| \\ | |/ _ \\  |  ___|_ _| |   | ____/ ___|  / / |  _ \\_ _|  _ \\/ ___|     ",
    "|  \\| | | | | | |_   | || |   |  _| \\___ \\ / /  | | | | || |_) \\___ \\     ",
    "| |\\  | |_| | |  _|  | || |___| |___ ___) / /   | |_| | ||  _ < ___) |  _ ",
    "|_| \\_|\\___/  |_|   |___|_____|_____|____/_/    |____/___|_| \\_\\____/  (_)",
    " "
};

typedef struct {
    char name[NAME_MAX];
    int is_dir;
} FileItem;

typedef struct {
    char path[PATH_MAX];
    int highlight;
} DirHistory;

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

const char *get_file_extension(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

void print_items(WINDOW *win, FileItem items[], int count, int highlight, const char *current_path, int show_hidden, int scroll_position, int height) {
    char *hidden_dir;
    if (show_hidden) {
        hidden_dir = "ON\u25C6";
    } else {
        hidden_dir = "OFF\u25C7";
    }
    // getting system free space from / dir 
    double systemFreeSpace = system_free_space("/");

    color_pair_init(); 

    // Print title
    wattron(win, COLOR_PAIR(9));
    char* cur_user = get_current_user();
    char* cur_hostname = get_hostname();
    wattron(win, A_BOLD | COLOR_PAIR(4));
    mvwprintw(win, 0, 2, "\u2502 📁 LITE FM: \u2502");
    wattroff(win, A_BOLD | COLOR_PAIR(4));

    char sanitizedCurPath[PATH_MAX];
    if (strncmp(current_path, "//", 2) == 0) {
      snprintf(sanitizedCurPath, sizeof(sanitizedCurPath), "%s", current_path + 1);
    } else {
    strcpy(sanitizedCurPath, current_path);
  }

    // Print current path and hidden directories status
    wattron(win, A_BOLD);
    wattron(win, COLOR_PAIR(8));
    mvwprintw(win, 2, 2, " %s@%s ", cur_hostname, cur_user);
    wattroff(win, COLOR_PAIR(8));
    wattron(win, COLOR_PAIR(9));
    mvwprintw(win, 2, 20, " %s ", sanitizedCurPath);
    wattroff(win, COLOR_PAIR(9));
    wattron(win, COLOR_PAIR(9));
    mvwprintw(win, LINES - 3, (COLS / 2) - 40, " Hidden Dirs: %s ", hidden_dir);
    mvwprintw(win, LINES - 3, (COLS / 2) - 18, " 💾 %.2f GiB ", systemFreeSpace);
    wattroff(win, COLOR_PAIR(9));
    wattroff(win, A_BOLD);

    // Print items
    if (count == 0) {
        wattron(win, COLOR_PAIR(3));
        int err_msg_size = sizeof(err_message) / sizeof(err_message[0]);
        for (int i=0;i<err_msg_size;i++) {
          mvwprintw(win, i+5, 2, "%s", err_message[i]);
        }
        wattroff(win, COLOR_PAIR(3));
        wattron(win, A_BOLD | COLOR_PAIR(5));
        mvwprintw(win, 15, 2, "<== PRESS H or <-");
        wattroff(win, A_BOLD | COLOR_PAIR(5));
    } else {
        for (int i = 0; i < height - 7 && i + scroll_position < count; i++) {
            int index = i + scroll_position;
            if (index == highlight)
                wattron(win, A_REVERSE);

            // Apply color based on file type
            if (items[index].is_dir) {
                wattron(win, COLOR_PAIR(2));
                mvwprintw(win, i+4, 5, "📁");
            } else {
                // Determine file type by extension
                char *extension = strrchr(items[index].name, '.');
                if (extension) {
                    if (strcmp(extension, ".zip") == 0 || strcmp(extension, ".tar") == 0 || strcmp(extension, ".xz") == 0 || strcmp(extension, ".gz") == 0 || strcmp(extension, ".jar") == 0) {
                        wattron(win, COLOR_PAIR(3));
                        mvwprintw(win, i+4, 5, "📦");
                    } else if (strcmp(extension, ".mp3") == 0 || strcmp(extension, ".mp4") == 0 || strcmp(extension, ".wav") == 0 || strcmp(extension, ".flac") == 0 || strcmp(extension, ".opus") == 0) {
                        wattron(win, COLOR_PAIR(4));
                        mvwprintw(win, i+4, 5, "🎵 ");
                    } else if (strcmp(extension, ".png") == 0 || strcmp(extension, ".jpg") == 0 || strcmp(extension, ".webp") == 0 || strcmp(extension, ".gif") == 0) {
                        wattron(win, COLOR_PAIR(5));
                        mvwprintw(win, i+4, 5, "🖼️");
                    } else {
                        wattron(win, COLOR_PAIR(1));
                        mvwprintw(win, i+4, 5, "📄");
                    }
                } else {
                    wattron(win, COLOR_PAIR(1));
                    mvwprintw(win, i+4, 5, "📄");
                }
            }
            wattron(win, A_BOLD);

            mvwprintw(win, i + 4, 7, " %s ", items[index].name);

            // Turn off color attributes
            wattroff(win, A_BOLD);
            wattroff(win, COLOR_PAIR(1));
            wattroff(win, COLOR_PAIR(2));
            wattroff(win, COLOR_PAIR(3));
            wattroff(win, COLOR_PAIR(4));
            wattroff(win, COLOR_PAIR(5));
            wattroff(win, COLOR_PAIR(9));

            if (index == highlight)
                wattroff(win, A_REVERSE);
        }
    }
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

int find_previous_item(const char *query, FileItem items[], int item_count, int *start_index) {
    char lower_query[NAME_MAX];
    for (int i = 0; query[i] && i < NAME_MAX; i++) {
        lower_query[i] = tolower(query[i]);
    }
    lower_query[strlen(query)] = '\0';

    for (int i = *start_index; i >= 0; i--) {
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
    for (int i = item_count - 1; i > *start_index; i--) {
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

void get_file_info_popup(WINDOW *main_win, const char *path, const char *filename) {
    struct stat file_stat;
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, filename);

    // Get file information
    if (stat(full_path, &file_stat) == -1) {
        log_message(LOG_LEVEL_ERROR, "Error retrieving file information for %s", full_path);
        show_message(main_win, "Error retrieving file information.");
        return;
    }

    // Create a new window for displaying file information
    int info_win_height = 13;
    int info_win_width = (COLS / 3) + 4;
    int info_win_y = (LINES - info_win_height) / 2;
    int info_win_x = (COLS - info_win_width) / 2;

    WINDOW *info_win = newwin(info_win_height, info_win_width, info_win_y, info_win_x);
    draw_3d_info_win(info_win, info_win_y, info_win_x, info_win_height, info_win_width, 1, 2);
    box(info_win, 0, 0);

    // Display file information with colors
    wattron(info_win, COLOR_PAIR(1));
    mvwprintw(info_win, 1, 2, "File Information:");
    wattroff(info_win, COLOR_PAIR(1));

    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 3, 2, "Name: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", filename);
    wattroff(info_win, COLOR_PAIR(4));

    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 4, 2, "Size: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", format_file_size(file_stat.st_size));
    wattroff(info_win, COLOR_PAIR(4));

    const char *file_ext = strrchr(filename, '.');
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 5, 2, "Extension: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    if (file_ext != NULL) {
        wprintw(info_win, "%s", file_ext + 1);
    } else {
        wprintw(info_win, "none");
    }
    wattroff(info_win, COLOR_PAIR(4));

    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 6, 2, "Last Modified: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
    wprintw(info_win, "%s", mod_time);
    wattroff(info_win, COLOR_PAIR(4));

    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 7, 2, "Permissions: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, (S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IRUSR) ? "r" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IWUSR) ? "w" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IXUSR) ? "x" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IRGRP) ? "r" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IWGRP) ? "w" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IXGRP) ? "x" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IROTH) ? "r" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IWOTH) ? "w" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IXOTH) ? "x" : "-");
    wattroff(info_win, COLOR_PAIR(4));

    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 8, 2, "Inode: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%lu", file_stat.st_ino);
    wattroff(info_win, COLOR_PAIR(4));

    // Additional file attributes can be displayed here
    struct passwd *pwd = getpwuid(file_stat.st_uid);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 9, 2, "Owner: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s (%d)", pwd->pw_name, file_stat.st_uid);
    wattroff(info_win, COLOR_PAIR(4));

    wattron(info_win, COLOR_PAIR(2));
    mvwprintw(info_win, info_win_height - 2, 2, "Press any key to close this window.");
    wattroff(info_win, COLOR_PAIR(2));
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

void get_file_info(WINDOW *info_win, const char *path, const char *filename) {
    struct stat file_stat;
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", path, filename);

    // Get file information
    if (stat(full_path, &file_stat) == -1) {
        log_message(LOG_LEVEL_ERROR, "Error retrieving file information for %s", full_path);
        show_message(info_win, "Error retrieving file information.");
        box(info_win, 0, 0);
        wrefresh(info_win);
        return;
    }

    // Display file information
    wattron(info_win, A_BOLD);
    mvwprintw(info_win, 1, 2, "File/Dir Information:");
    wattroff(info_win, A_BOLD);

    clearLine(info_win, 3, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 3, 2, "Name: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", filename);
    wattroff(info_win, COLOR_PAIR(4));

    clearLine(info_win, 4, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 4, 2, "Size: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", format_file_size(file_stat.st_size));
    wattroff(info_win, COLOR_PAIR(4));

    const char *file_ext = strrchr(filename, '.');
    clearLine(info_win, 5, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 5, 2, "Extension: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    if (file_ext != NULL) {
        wprintw(info_win, "%s", file_ext + 1);
    } else {
        wprintw(info_win, "none");
    }
    wattroff(info_win, COLOR_PAIR(4));

    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
    clearLine(info_win, 6, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 6, 2, "Last Modified: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", mod_time);
    wattroff(info_win, COLOR_PAIR(4));

    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 7, 2, "Inode: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%lu", file_stat.st_ino);
    wattroff(info_win, COLOR_PAIR(4));

    clearLine(info_win, 8, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 8, 2, "Type: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(5));
    if (S_ISREG(file_stat.st_mode)) {
        wprintw(info_win, "Regular File");
    } else if (S_ISDIR(file_stat.st_mode)) {
        wattron(info_win, COLOR_PAIR(11));
        wprintw(info_win, "Directory");
        wattroff(info_win, COLOR_PAIR(11));
        // Count and display subdirectories
        DIR *dir;
        struct dirent *entry;
        int line = 14;
        dir = opendir(full_path);
        if (dir != NULL) {
            wattron(info_win, A_BOLD | COLOR_PAIR(9));
            mvwprintw(info_win, line++, 2, " Subdirectories: ");
            wattroff(info_win, A_BOLD | COLOR_PAIR(9));
            wattron(info_win, A_BOLD | COLOR_PAIR(2));
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_DIR) {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        mvwprintw(info_win, line++, 2, "  %s", entry->d_name);
                    }
                }
            }
            closedir(dir);
            wattroff(info_win, A_BOLD | COLOR_PAIR(2));
        } else {
            show_message(info_win, "Error opening directory.");
        }
    } else if (S_ISLNK(file_stat.st_mode)) {
        wprintw(info_win, "Symbolic Link");
    } else if (S_ISFIFO(file_stat.st_mode)) {
        wprintw(info_win, "FIFO");
    } else if (S_ISCHR(file_stat.st_mode)) {
        wprintw(info_win, "Character Device");
    } else if (S_ISBLK(file_stat.st_mode)) {
        wprintw(info_win, "Block Device");
    } else if (S_ISSOCK(file_stat.st_mode)) {
        wprintw(info_win, "Socket");
    } else {
        wprintw(info_win, "Unknown");
    }
    wattroff(info_win, COLOR_PAIR(5));

    clearLine(info_win, 9, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 9, 2, "Permissions: ");
    wattroff(info_win, COLOR_PAIR(3));
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, (S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IRUSR) ? "r" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IWUSR) ? "w" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IXUSR) ? "x" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IRGRP) ? "r" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IWGRP) ? "w" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IXGRP) ? "x" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IROTH) ? "r" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IWOTH) ? "w" : "-");
    wprintw(info_win, (file_stat.st_mode & S_IXOTH) ? "x" : "-");
    wattroff(info_win, COLOR_PAIR(4));

    clearLine(info_win, 10, 2);
    struct passwd *pwd = getpwuid(file_stat.st_uid);
    wattron(info_win, COLOR_PAIR(6));
    mvwprintw(info_win, 10, 2, "Owner: %s (%d)", pwd->pw_name, file_stat.st_uid);
    wattroff(info_win, COLOR_PAIR(6));
    
    struct group *grp = getgrgid(file_stat.st_gid);
    if (grp != NULL) {
        wattron(info_win, COLOR_PAIR(6));
        mvwprintw(info_win, 11, 2, "Group: %s (%d)", grp->gr_name, file_stat.st_gid);
        wattroff(info_win, COLOR_PAIR(6));
    } else {
        wattron(info_win, COLOR_PAIR(6));
        mvwprintw(info_win, 11, 2, "Group: %d", file_stat.st_gid);
        wattroff(info_win, COLOR_PAIR(6));
    }

    wrefresh(info_win);
    // Refresh the main window to ensure no artifacts remain
    box(info_win, 0, 0);
    wrefresh(info_win);
}

void handle_rename(WINDOW *win, const char *path) {
    char new_name[PATH_MAX];
    char old_name[PATH_MAX];
    
    // Copy the original filename
    strncpy(old_name, basename((char *)path), PATH_MAX);
    
    // Prompt for new name
    get_user_input_from_bottom(win, new_name, sizeof(new_name), "rename");

    // Check if new_name is not empty
    if (strlen(new_name) == 0) {
        show_term_message("No name provided. Aborting rename.", 1);
        return;
    }

    // Extract file extensions
    const char *old_ext = get_file_extension(old_name);
    const char *new_ext = get_file_extension(new_name);

    // Check if extensions are different
    if (strcmp(old_ext, new_ext) != 0) {
        show_term_message("Extension change not allowed. Aborting rename.", 1);
        return;
    }

    // Perform rename
    if (rename_file_or_dir(path, new_name) == 0) {
        log_message(LOG_LEVEL_INFO, "Rename successful for %s", path); 
        show_term_message("Rename successful.", 0);
        // Update file list if needed
    } else {
        log_message(LOG_LEVEL_ERROR, "Rename error for %s", path);
        show_term_message("Rename failed.", 1);
    }
}

int main() {
    init_curses(); 

    int highlight = 0;
    FileItem items[MAX_ITEMS];
    int item_count = 0;
    char current_path[PATH_MAX];
    DirHistory history[MAX_HISTORY];
    int history_count = 0;
    int show_hidden = 0;  // Flag to toggle showing hidden files
    int scroll_position = 0;  // Position of the first visible item
 
    get_current_working_directory(current_path, sizeof(current_path));
    list_dir(current_path, items, &item_count, show_hidden);

    // Create a new window with a border
    int startx = 0, starty = 0;
    int width = COLS / 2, height = LINES - 1;
    WINDOW *win = newwin(height, width, starty, startx);
    box(win, 0, 0);

    int info_startx = COLS / 2, info_starty = 0;
    int info_width = COLS / 2, info_height = LINES - 1;
    WINDOW *info_win = newwin(info_height, info_width, info_starty, info_startx);
    box(info_win, 0, 0);

    // Initial display
    print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
    wrefresh(win);
    wrefresh(info_win);

    timeout(1);
    nodelay(win, TRUE);

    int find_index = 0;
    char last_query[NAME_MAX] = "";
    bool firstKeyPress = true;
    log_message(LOG_LEVEL_DEBUG, "================ LITEFM INSTANCE STARTED =================");

    while (true) {
        int choice = getch();
        if (firstKeyPress) {
            werase(win);
            box(info_win, 0, 0);
            box(win, 0, 0);
            print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
            wrefresh(win);
            werase(info_win);
            char full_path_info[PATH_MAX];
            snprintf(full_path_info, PATH_MAX, "%s/%s", current_path, items[highlight].name);
            if (is_readable_extension(items[highlight].name)) { 
                display_file(info_win, full_path_info);
            } else { 
              get_file_info(info_win, current_path, items[highlight].name);
            }
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
                        WINDOW *input_win = newwin(3, (COLS / 2) - (COLS / 3), LINES - 5, 2);
                        box(input_win, 0, 0);
                        mvwprintw(input_win, 0, 1, " \u25B2 %s/ ", current_path);
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
                                    log_message(LOG_LEVEL_INFO, "Directory created successfully for %s", name_input);
                                    char msg[256];
                                    snprintf(msg, sizeof(msg), "Directory '%s' created at %s.", name_input, timestamp);
                                    show_term_message(msg,0);
                                } else if (result == 1) {
                                    log_message(LOG_LEVEL_WARN, "Directory %s already exists", name_input);
                                    show_term_message("Directory already exists.",1);
                                } else {
                                    log_message(LOG_LEVEL_ERROR, "Error creating directory for %s", name_input);
                                    show_term_message("Error creating directory.",1);
                                }
                            } else {
                                // Create file
                                int result = create_file(current_path, name_input, timestamp);
                                if (result == 0) {
                                    char msg[256];
                                    log_message(LOG_LEVEL_INFO, "File created successfully for %s", name_input);
                                    snprintf(msg, sizeof(msg), "File '%s' created at %s.", name_input, timestamp);
                                    show_term_message(msg,0);
                                } else if (result == 1) {
                                    log_message(LOG_LEVEL_WARN, "File %s already exists", name_input);
                                    show_term_message("File already exists.",1);
                                } else {
                                    log_message(LOG_LEVEL_ERROR, "Error creating file for %s", name_input);
                                    show_term_message("Error creating file.",1);
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
                                        log_message(LOG_LEVEL_ERROR, "Error deleting directory for %s", items[highlight].name);
                                        show_term_message("Error removing directory. Dir might be recursive.",1);
                                    } else {
                                        char delmsg[256];
                                        log_message(LOG_LEVEL_INFO, "Directory %s deleted", deldir);
                                        snprintf(delmsg, sizeof(delmsg), "Directory '%s' deleted", deldir);
                                        show_term_message(delmsg,0);
                                     } 
                                } else {
                                    char *delfile = items[highlight].name;
                                    int result = remove_file(current_path, items[highlight].name);
                                    if (result != 0) {
                                      log_message(LOG_LEVEL_ERROR, "Error removing file for %s", items[highlight].name);
                                      show_term_message("Error removing file.",1);
                                    } else {
                                        char msg[256];
                                        log_message(LOG_LEVEL_INFO, "File %s deleted", delfile);
                                        snprintf(msg, sizeof(msg), "File '%s' deleted", delfile);
                                        show_term_message(msg,0);
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
                               log_message(LOG_LEVEL_WARN, "Attempted to remove file %s recursively.", items[highlight].name);
                               show_term_message("This command is for deleting recursive directories ONLY!", 1);
                               break;
                            }

                            if (confirm_action(win, confirm_msg)) {
                                char *deldir = items[highlight].name;
                                if (items[highlight].is_dir) {
                                    int parent_fd = open(current_path, O_RDONLY | O_DIRECTORY);
                                    int result = remove_directory_recursive(current_path, items[highlight].name, parent_fd);
                                    if (result != 0) {
                                        log_message(LOG_LEVEL_ERROR, "Error removing directory %s", items[highlight].name);
                                        show_term_message("Error removing directory.",1);
                                    } else {
                                        char delmsg[256];
                                        log_message(LOG_LEVEL_INFO, "Directory %s deleted recursively", deldir);
                                        snprintf(delmsg, sizeof(delmsg), "Directory '%s' deleted recursively", deldir);
                                        show_term_message(delmsg,0);
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
                        wattron(win, A_BOLD | COLOR_PAIR(7));
                        mvwprintw(win, LINES - 3, (COLS / 2) - 55, "🔍 Search ON ");
                        wattroff(win, A_BOLD | COLOR_PAIR(7));
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
                                show_term_message("Item not found.", 1);
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
                            log_message(LOG_LEVEL_WARN, "No more NEXT occurances for %s found", last_query);
                            show_term_message("No more occurrences found.",1);
                        }
                    }
                    break;
               case 'N':
                  if (strlen(last_query) > 0) {
                      int start_index = highlight - 1;
                      int found_index = find_previous_item(last_query, items, item_count, &start_index);
                      if (found_index != -1) {
                          highlight = found_index;
                          if (highlight >= scroll_position + height - 8) {
                              scroll_position = highlight - height + 8;
                          } else if (highlight < scroll_position) {
                              scroll_position = highlight;
                          }
                      } else {
                          log_message(LOG_LEVEL_WARN, "No more PREV occurances for %s found", last_query);
                          show_term_message("No previous occurrences found.", 1);
                      }
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
                                log_message(LOG_LEVEL_INFO, "Extraction of %s successful", full_path);
                                show_term_message("Extraction successful.",0);
                                // Update file list after extraction
                                list_dir(current_path, items, &item_count, show_hidden);
                                scroll_position = 0;
                            } else {
                                log_message(LOG_LEVEL_ERROR, "Extraction of %s failed", last_query);
                                show_term_message("Extraction failed.",1);
                            }
                        }
                    } else {
                        log_message(LOG_LEVEL_WARN, "File %s is not a supported archive format", filename);
                        show_term_message("Not a supported archive format.",1);
                    }
                } else {
                  log_message(LOG_LEVEL_ERROR, "Cannot extract %s as it is is directory", items[highlight].name);
                  show_term_message("Cannot extract a directory.",1);
                }
                break;
               
            
            case 'Z':
                if (items[highlight].is_dir) {
                    // Check if the selected item is a directory
                    const char *dirname = items[highlight].name;
                    char full_path[PATH_MAX];
                    snprintf(full_path, PATH_MAX, "%s/%s", current_path, dirname);

                    // Ask user for the compression format
                    int choice = show_compression_options(win); // Implement this function to show options and get the user's choice

                    // Define the output archive path
                    char archive_path[PATH_MAX];
                    snprintf(archive_path, PATH_MAX, "%s/%s.%s", current_path, dirname, (choice == 1) ? "tar" : "zip");

                    // Confirm compression
                    if (choice == 1 || choice == 2) {
                        // Compress the directory based on the user's choice
                        int result;
                        if (choice == 1) {
                            // Compress as TAR
                            result = compress_directory(full_path, archive_path, 1);
                        } else {
                            // Compress as ZIP
                            result = compress_directory(full_path, archive_path, 2);
                        }

                        if (result == 0) {
                            log_message(LOG_LEVEL_INFO, "Compression of %s (compression type: %d) successful", dirname, choice);
                            show_term_message("Compression successful.", 0);
                            // Update file list after compression
                            list_dir(current_path, items, &item_count, show_hidden);
                            scroll_position = 0;
                        } else {
                            log_message(LOG_LEVEL_ERROR, "Compression of %s (compression type: %d) failed", dirname, choice);
                            show_term_message("Compression failed.", 1);
                        }
                    }
                } else {
                    log_message(LOG_LEVEL_WARN, "Selected item %s is not a directory", items[highlight].name);
                    show_term_message("Selected item is not a directory.", 1);
                }
                break;
               
              case 'R': // Rename file or directory
              {
                  if (item_count > 0) {
                      const char *current_name = items[highlight].name;
                      char full_path[PATH_MAX];
                      snprintf(full_path, PATH_MAX, "%s/%s", current_path, current_name);

                      char new_name[NAME_MAX];
                      get_user_input_from_bottom(stdscr, new_name, NAME_MAX, "rename");

                      if (strlen(new_name) > 0) {
                          char new_full_path[PATH_MAX];
                          snprintf(new_full_path, PATH_MAX, "%s/%s", current_path, new_name);

                          if (rename(full_path, new_full_path) == 0) {
                              log_message(LOG_LEVEL_INFO, "Rename of %s to %s was successful", current_name, new_name);
                              show_term_message("Rename successful.", 0);
                              // Update file list after renaming
                              list_dir(current_path, items, &item_count, show_hidden);
                              scroll_position = 0;
                          } else {
                              log_message(LOG_LEVEL_ERROR, "Rename of %s to %s failed", current_name, new_name);
                              show_term_message("Rename failed.", 1);
                          }
                      } else {
                          log_message(LOG_LEVEL_WARN, "No name provided to change for %s, so renaming failed", current_name);
                          show_term_message("No name provided. Aborting rename.", 1);
                      }
                  } else {
                      log_message(LOG_LEVEL_WARN, "No item selected for renaming");
                      show_term_message("No item selected for renaming.", 1);
                  }
                  break;
              }
               case 10:
                 if (is_readable_extension(items[highlight].name)) {
                    get_file_info_popup(win, current_path, items[highlight].name);
                } else {
                    log_message(LOG_LEVEL_DEBUG, "Something went wrong...");
                    show_term_message("Something went wrong....",1);      
                }
                break;
                case 'q':
                    log_message(LOG_LEVEL_DEBUG, "================ LITEFM INSTANCE OVER =================");
                    endwin();
                    return 0;
            } 
            // Update display after each key press 
            werase(win);
            box(win, 0, 0); 
            print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
            wrefresh(win);
            werase(info_win);
            char full_path_info[PATH_MAX];
            snprintf(full_path_info, PATH_MAX, "%s/%s", current_path, items[highlight].name);
            if (item_count > 0) {
              if (is_readable_extension(items[highlight].name)) {
                  display_file(info_win, full_path_info);
              } else { 
                get_file_info(info_win, current_path, items[highlight].name);
              }
           }
        }
    }

    endwin();
    return 0;
}
