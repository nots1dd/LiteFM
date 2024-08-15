// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/*
 * ---------------------------------------------------------------------------
 *  File:        lfm.c
 *  Description: This is main C file that encompasses
 *               all the functionalities of LiteFM
 *
 *  Author:      nots1dd
 *  Created:     <31/07/24>
 * 
 *  Copyright:   2024 Siddharth Karanam. All rights reserved.
 * 
 *  License:     <GNU GPL v3>
 *
 *  Notes:      Some cool features like history/file tracker,
 *              is done through typedef structs
 *
 *  Includes:   `ncurses library`, `libarchive`, `statvfs, stat`,
 *              `dirent`, `sys headers`, `string headers`, `grp`,
 *              `std headers`, `unistd`, `fcntl` so on...
 * 
 *  Revision History:
 *      <31/07/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#include <sys/types.h>
#include <fcntl.h>
#include <grp.h>

/* LITEFM DEDICATED HEADERS */

#include "src/getFreeSpace.c"
#include "include/cursesutils.h"
#include "include/filepreview.h"
#include "include/dircontrol.h"
#include "include/archivecontrol.h"
#include "include/logging.h"
#include "include/clipboard.h"
#include "include/signalhandling.h"
#include "include/syntax.h"
#include "include/hashtable.h"

#define MAX_ITEMS 1024
#define MAX_HISTORY 256

const char * err_message[] = {
  " _   _  ___    _____ ___ _     _____ ____    __  ____ ___ ____  ____      ",
  "| \\ | |/ _ \\  |  ___|_ _| |   | ____/ ___|  / / |  _ \\_ _|  _ \\/ ___|     ",
  "|  \\| | | | | | |_   | || |   |  _| \\___ \\ / /  | | | | || |_) \\___ \\     ",
  "| |\\  | |_| | |  _|  | || |___| |___ ___) / /   | |_| | ||  _ < ___) |  _ ",
  "|_| \\_|\\___/  |_|   |___|_____|_____|____/_/    |____/___|_| \\_\\____/  (_)",
  " "
};

const char *denied_message[] = {
" .__   __.   ______           _______. __    __      ",
" |  \\ |  |  /  __  \\         /       ||  |  |  |     ",
" |   \\|  | |  |  |  |       |   (----`|  |  |  |     ",
" |  . `  | |  |  |  |        \\   \\    |  |  |  |     ",
" |  |\\   | |  `--'  |    .----)   |   |  `--'  |  __ ",
" |__| \\__|  \\______/     |_______/     \\______/  (__)",
"                                                     "
};


typedef struct {
  char name[NAME_MAX];
  int is_dir;
}
FileItem;

typedef struct {
  char path[PATH_MAX];
  int highlight;
}
DirHistory;

void truncate_symlink_name(char *name);

void list_dir(WINDOW *win, const char *path, FileItem items[], int *count, int show_hidden) {
    DIR *dir;
    struct dirent *entry;
    char full_path[PATH_MAX];
    struct stat entry_stat;
    char symlink_target[PATH_MAX];

    werase(win);

    if (!(dir = opendir(path)))
        return;

    *count = 0;

    // First pass: List directories and symlinks
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files if not showing hidden files
        if ((!show_hidden && entry->d_name[0] == '.') || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Build the full path for the current entry
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Get file information
        if (lstat(full_path, &entry_stat) == -1) {
            // Handle error if necessary
            continue;
        }

        if (S_ISLNK(entry_stat.st_mode)) {
            // If it's a symlink, get the target
            ssize_t len = readlink(full_path, symlink_target, sizeof(symlink_target) - 1);
            if (len != -1) {
                symlink_target[len] = '\0';  // Null-terminate the symlink target string
                snprintf(items[*count].name, sizeof(items[*count].name), "%s ->%s", entry->d_name, symlink_target);
            } else {
                snprintf(items[*count].name, sizeof(items[*count].name), "%s ->unknown", entry->d_name);
            }
            items[*count].is_dir = (entry_stat.st_mode & S_IFDIR) ? 1 : 0;
            (*count)++;
        } else if (entry->d_type == DT_DIR) {
            strcpy(items[*count].name, entry->d_name);
            items[*count].is_dir = 1;
            (*count)++;
        }
    }

    // Rewind directory stream for the second pass
    rewinddir(dir);

    // Second pass: List non-directory files
    while ((entry = readdir(dir)) != NULL) {
        // Skip hidden files if not showing hidden files
        if ((!show_hidden && entry->d_name[0] == '.') || strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        // Build the full path for the current entry
        snprintf(full_path, sizeof(full_path), "%s/%s", path, entry->d_name);

        // Get file information
        if (lstat(full_path, &entry_stat) == -1) {
            // Handle error if necessary
            continue;
        }

        if (!S_ISLNK(entry_stat.st_mode) && entry->d_type != DT_DIR) {
            strcpy(items[*count].name, entry->d_name);
            items[*count].is_dir = 0;
            (*count)++;
        }
    }

    closedir(dir);
    wrefresh(win);
}

int is_symlink(const char *path) {
    struct stat statbuf;
    char tmpPath[MAX_PATH_LENGTH];
    strcpy(tmpPath, path);
    truncate_symlink_name(tmpPath);
    // Use lstat to check for symbolic links
    if (lstat(tmpPath, &statbuf) == 0) {
        if (S_ISLNK(statbuf.st_mode)) {
            return 1; // Return 1 (true) if it's a symbolic link
        }
    }

    return 0; // Return 0 (false) if it's not a symbolic link or lstat fails
}

void print_items(WINDOW *win, FileItem items[], int count, int highlight,
                 const char *current_path, int show_hidden, int scroll_position, int height) {
    char *hidden_dir;
    if (show_hidden) {
        hidden_dir = "ON\u25C6";
    } else {
        hidden_dir = "OFF\u25C7";
    }

    // Getting system free space from / dir
    double systemFreeSpace = system_free_space("/");
    double totalSystemSpace = system_total_space("/");

    color_pair_init();

    // Print title
    wattron(win, COLOR_PAIR(9));
    char *cur_user = get_current_user();
    char *cur_hostname = get_hostname();
    wattron(win, A_BOLD | COLOR_PAIR(14));
    mvwprintw(win, 0, 2, " 🗄️ LITE FM: ");
    wattroff(win, A_BOLD | COLOR_PAIR(14));

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
    print_limited(win, 2, 20, sanitizedCurPath);
    wattroff(win, COLOR_PAIR(9));
    wattron(win, COLOR_PAIR(9));
    mvwprintw(win, LINES - 3, (COLS / 2) - 55, " Hidden Dirs: %s ", hidden_dir);
    mvwprintw(win, LINES - 3, (COLS / 2) - 30, " 💾 %.2f / %.2f GiB ", systemFreeSpace, totalSystemSpace);
    wattroff(win, COLOR_PAIR(9));
    wattroff(win, A_BOLD);

    // Print items
    if (count == 0) {
        wattron(win, COLOR_PAIR(3));
        int err_msg_size = sizeof(err_message) / sizeof(err_message[0]);
        for (int i = 0; i < err_msg_size; i++) {
            mvwprintw(win, i + 5, 2, "%s", err_message[i]);
        }
        wattroff(win, COLOR_PAIR(3));
        wattron(win, A_BOLD | COLOR_PAIR(5));
        mvwprintw(win, 15, 2, "<== PRESS H or <-");
        wattroff(win, A_BOLD | COLOR_PAIR(5));
    } else {
        for (int i = 0; i < height - 7 && i + scroll_position < count; i++) {
            int index = i + scroll_position;
            if (index == highlight) {
                wattron(win, A_REVERSE);
            }
            char full_path[MAX_PATH_LENGTH];
            snprintf(full_path, sizeof(full_path), "%s/%s", current_path, items[index].name);
            // Apply color based on file type
            if (items[index].is_dir) {
                wattron(win, COLOR_PAIR(2));
                mvwprintw(win, i + 4, 5, "📁");
            } else if (is_symlink(full_path)) {
                wattron(win, COLOR_PAIR(6));  // Choose an appropriate color pair for symlinks
                mvwprintw(win, i + 4, 5, "🔗"); // Unicode for symlink
            } else {
                // Determine file type by extension
                char *extension = strrchr(items[index].name, '.');
                if (extension) {
                    if (strcmp(extension, ".zip") == 0 || strcmp(extension, ".7z") == 0 || strcmp(extension, ".tar") == 0 || strcmp(extension, ".xz") == 0 || strcmp(extension, ".gz") == 0 || strcmp(extension, ".jar") == 0) {
                        wattron(win, COLOR_PAIR(3));
                        mvwprintw(win, i + 4, 5, "📦");
                    } else if (strcmp(extension, ".mp3") == 0 || strcmp(extension, ".mp4") == 0 || strcmp(extension, ".wav") == 0 || strcmp(extension, ".flac") == 0 || strcmp(extension, ".opus") == 0) {
                        wattron(win, COLOR_PAIR(4));
                        mvwprintw(win, i + 4, 5, "🎵 ");
                    } else if (strcmp(extension, ".png") == 0 || strcmp(extension, ".jpg") == 0 || strcmp(extension, ".webp") == 0 || strcmp(extension, ".gif") == 0) {
                        wattron(win, COLOR_PAIR(5));
                        mvwprintw(win, i + 4, 5, "🖼️");
                    } else {
                        wattron(win, COLOR_PAIR(1));
                        mvwprintw(win, i + 4, 5, "📄");
                    }
                } else {
                    wattron(win, COLOR_PAIR(1));
                    mvwprintw(win, i + 4, 5, "📄");
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
            wattroff(win, COLOR_PAIR(6)); // Turn off the color pair for symlinks
            wattroff(win, COLOR_PAIR(9));

            if (index == highlight)
                wattroff(win, A_REVERSE);
        }
    }
}

/*
 * TRUNCATING A SYMLINK NAME IS NECESSARY AS THE ITEM NAME MIGHT HAVE 
 * THE STRING QUERY WE ARE LOOKING FOR,
 *
 * EXAMPLE: LIB64 ->LIB 
 * IF I WANT TO SEARCH FOR LIB, I SHOULD GO TO THE ITEM THAT ACTUALLY 
 * HAS THE STRING `LIB`, AND NOT A SYMLINK OF IT.
 * 
*/
void truncate_symlink_name(char *name) {
    char *arrow = strstr(name, " ->");
    if (arrow) {
        *arrow = '\0'; // Truncate the name at the " -> " point
    }
}

int find_item(const char *query, FileItem items[], int item_count, int *start_index, int direction) {
    char lower_query[NAME_MAX];
    for (int i = 0; query[i] && i < NAME_MAX; i++) {
        lower_query[i] = tolower(query[i]);
    }
    lower_query[strlen(query)] = '\0'; 

    if (direction == 1) { // Forward search
        for (int i = *start_index; i < item_count; i++) {
            char truncated_name[NAME_MAX];
            strcpy(truncated_name, items[i].name);
            truncate_symlink_name(truncated_name);

            char lower_name[NAME_MAX];
            for (int j = 0; truncated_name[j] && j < NAME_MAX; j++) {
                lower_name[j] = tolower(truncated_name[j]);
            }
            lower_name[strlen(truncated_name)] = '\0';

            if (strstr(lower_name, lower_query) != NULL) {
                *start_index = i;
                return i;
            }
        }
        for (int i = 0; i < *start_index; i++) {
            char truncated_name[NAME_MAX];
            strcpy(truncated_name, items[i].name);
            truncate_symlink_name(truncated_name);

            char lower_name[NAME_MAX];
            for (int j = 0; truncated_name[j] && j < NAME_MAX; j++) {
                lower_name[j] = tolower(truncated_name[j]);
            }
            lower_name[strlen(truncated_name)] = '\0';

            if (strstr(lower_name, lower_query) != NULL) {
                *start_index = i;
                return i;
            }
        }
    } else if (direction == -1) { // Backward search
        for (int i = *start_index; i >= 0; i--) {
            char truncated_name[NAME_MAX];
            strcpy(truncated_name, items[i].name);
            truncate_symlink_name(truncated_name);

            char lower_name[NAME_MAX];
            for (int j = 0; truncated_name[j] && j < NAME_MAX; j++) {
                lower_name[j] = tolower(truncated_name[j]);
            }
            lower_name[strlen(truncated_name)] = '\0';

            if (strstr(lower_name, lower_query) != NULL) {
                *start_index = i;
                return i;
            }
        }
        for (int i = item_count - 1; i > *start_index; i--) {
            char truncated_name[NAME_MAX];
            strcpy(truncated_name, items[i].name);
            truncate_symlink_name(truncated_name);

            char lower_name[NAME_MAX];
            for (int j = 0; truncated_name[j] && j < NAME_MAX; j++) {
                lower_name[j] = tolower(truncated_name[j]);
            }
            lower_name[strlen(truncated_name)] = '\0';

            if (strstr(lower_name, lower_query) != NULL) {
                *start_index = i;
                return i;
            }
        }
    }

    return -1; // Not found
}

void get_file_info_popup(WINDOW * main_win,
  const char * path,
    const char * filename) {
  struct stat file_stat;
  char full_path[PATH_MAX];
  snprintf(full_path, PATH_MAX, "%s/%s", path, filename);

  // Get file information
  if (stat(full_path, & file_stat) == -1) {
    log_message(LOG_LEVEL_ERROR, "Error retrieving file information for `%s`", full_path);
    show_message(main_win, "Error retrieving file information.");
    return;
  }

  // Create a new window for displaying file information
  int info_win_height = 13;
  int info_win_width = (COLS / 3) + 4;
  int info_win_y = (LINES - info_win_height) / 2;
  int info_win_x = (COLS - info_win_width) / 2;

  WINDOW * info_win = newwin(info_win_height, info_win_width, info_win_y, info_win_x);
  draw_3d_info_win(info_win, info_win_y, info_win_x, info_win_height, info_win_width, 1, 2);
  box(info_win, 0, 0);

  // Display file information with colors
  colorLine(info_win, "File Information: ", 1, 1, 2); /* colorLine params: win, string, color_pair, x, y */
  colorLine(info_win, "Name: ", 3, 3, 2);

  wattron(info_win, COLOR_PAIR(4));
  wprintw(info_win, "%s", filename);
  wattroff(info_win, COLOR_PAIR(4));

  colorLine(info_win, "Size: ", 3, 4, 2);
  wattron(info_win, COLOR_PAIR(4));
  wprintw(info_win, "%s", format_file_size(file_stat.st_size));
  wattroff(info_win, COLOR_PAIR(4));

  const char * file_ext = strrchr(filename, '.');
  colorLine(info_win, "Extension: ", 3, 5, 2);
  wattron(info_win, COLOR_PAIR(4));
  if (file_ext != NULL) {
    wprintw(info_win, "%s", file_ext + 1);
  } else {
    wprintw(info_win, "none");
  }
  wattroff(info_win, COLOR_PAIR(4));

  colorLine(info_win, "Last Modified: ", 3, 6, 2);
  wattron(info_win, COLOR_PAIR(4));
  char mod_time[20];
  strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime( & file_stat.st_mtime));
  wprintw(info_win, "%s", mod_time);
  wattroff(info_win, COLOR_PAIR(4));

  colorLine(info_win, "Permissions: ", 3, 7, 2);
  print_permissions(info_win, &file_stat);

  colorLine(info_win, "Inode: ", 3, 8, 2);
  wattron(info_win, COLOR_PAIR(4));
  wprintw(info_win, "%lu", file_stat.st_ino);
  wattroff(info_win, COLOR_PAIR(4));

  // Additional file attributes can be displayed here
  struct passwd * pwd = getpwuid(file_stat.st_uid);
  wattron(info_win, COLOR_PAIR(3));
  mvwprintw(info_win, 9, 2, "Owner: ");
  wattroff(info_win, COLOR_PAIR(3));
  wattron(info_win, COLOR_PAIR(4));
  wprintw(info_win, "%s (%d)", pwd -> pw_name, file_stat.st_uid);
  wattroff(info_win, COLOR_PAIR(4));

  colorLine(info_win, "Press any key to close this window.", 2, info_win_height - 2, 2);
  wrefresh(info_win);

  wgetch(info_win); // Wait for user input
  wclear(info_win); // Clear info window before deleting
  delwin(info_win); // Delete info window

  // Refresh the main window to ensure no artifacts remain 
  wrefresh(main_win);
}

void get_file_info(WINDOW *info_win, const char *path, const char *filename) {
    werase(info_win);
    struct stat file_stat;
    char full_path[PATH_MAX];
    char truncated_file_name[PATH_MAX];
    char symlink_target[PATH_MAX];
    ssize_t len;

    strcpy(truncated_file_name, filename);
    truncate_symlink_name(truncated_file_name);
    snprintf(full_path, PATH_MAX, "%s/%s", path, truncated_file_name);

    // Get file information using lstat to handle symlinks
    if (lstat(full_path, &file_stat) == -1) {
        log_message(LOG_LEVEL_ERROR, "Error retrieving file information for %s", full_path);
        show_message(info_win, "Error retrieving file/dir info.");
        box(info_win, 0, 0);
        wrefresh(info_win);
        return;
    }

    if (access(full_path, R_OK) != 0) {
        log_message(LOG_LEVEL_ERROR, "Access denied for %s", full_path);
        int denied_message_size = sizeof(denied_message) / sizeof(denied_message[0]);
        for (int j = 0; j < denied_message_size; j++) {
            mvwprintw(info_win, j + 3, 2, "%s", denied_message[j]);
        }
        show_message(info_win, "Access denied for you!");
        box(info_win, 0, 0);
        wrefresh(info_win);
        return;
    }

    // Display file information
    wattron(info_win, A_BOLD);
    mvwprintw(info_win, 1, 2, "File/Dir Information:");
    wattroff(info_win, A_BOLD);

    clearLine(info_win, 3, 2);
    colorLine(info_win, "Name: ", 3, 3, 2);
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", filename);
    wattroff(info_win, COLOR_PAIR(4));

    clearLine(info_win, 4, 2);
    colorLine(info_win, "Size: ", 3, 4, 2);
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", format_file_size(file_stat.st_size));
    wattroff(info_win, COLOR_PAIR(4));

    const char *file_ext = strrchr(filename, '.');
    clearLine(info_win, 5, 2);
    colorLine(info_win, "Extension: ", 3, 5, 2);
    wattron(info_win, COLOR_PAIR(4));
    if (file_ext != NULL && !S_ISDIR(file_stat.st_mode)) {
        wprintw(info_win, "%s", file_ext + 1);
    } else {
        wprintw(info_win, "none");
    }
    wattroff(info_win, COLOR_PAIR(4));

    char mod_time[20];
    strftime(mod_time, sizeof(mod_time), "%Y-%m-%d %H:%M:%S", localtime(&file_stat.st_mtime));
    clearLine(info_win, 6, 2);
    colorLine(info_win, "Last Modified: ", 3, 6, 2);
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%s", mod_time);
    wattroff(info_win, COLOR_PAIR(4));

    colorLine(info_win, "Inode: ", 3, 7, 2);
    wattron(info_win, COLOR_PAIR(4));
    wprintw(info_win, "%lu", file_stat.st_ino);
    wattroff(info_win, COLOR_PAIR(4));

    clearLine(info_win, 8, 2);
    colorLine(info_win, "Group: ", 3, 8, 2);
    wattron(info_win, COLOR_PAIR(4));
    struct group *grp = getgrgid(file_stat.st_gid);
    if (grp != NULL) {
        wprintw(info_win, "%s", grp->gr_name);
    } else {
        wprintw(info_win, "Unknown");
    }
    wattroff(info_win, COLOR_PAIR(4));

    clearLine(info_win, 9, 2);
    colorLine(info_win, "Type: ", 3, 9, 2);
    wattron(info_win, COLOR_PAIR(5));
    if (S_ISREG(file_stat.st_mode)) {
        wprintw(info_win, "Regular File");

        // Check if the file is an archive
        if (file_ext != NULL && (strcmp(file_ext, ".zip") == 0 || strcmp(file_ext, ".7z") == 0 || strcmp(file_ext, ".tar") == 0 || strcmp(file_ext, ".gz") == 0)) {
            // Display archive contents
            display_archive_contents(info_win, full_path, file_ext);
        }
    } else if (S_ISDIR(file_stat.st_mode)) {
        wattron(info_win, COLOR_PAIR(11));
        wprintw(info_win, "Directory");
        wattroff(info_win, COLOR_PAIR(11));

        // Horizontal Layout for Parent Directories and Subdirectories
        wattron(info_win, A_BOLD | COLOR_PAIR(9));
        mvwprintw(info_win, 11, 2, " Parent Directories: ");
        mvwprintw(info_win, 11, getmaxx(info_win) / 2, "Subdirectories: ");
        wattroff(info_win, A_BOLD | COLOR_PAIR(9));

        char parent_dir[PATH_MAX];
        char current_path[PATH_MAX];
        strncpy(current_path, path, sizeof(current_path));
        current_path[sizeof(current_path) - 1] = '\0'; // Ensure null termination
        char *parent_dir_ptr = dirname(current_path);

        // Make a copy of the parent directory path to avoid modifying the original path
        strncpy(parent_dir, parent_dir_ptr, sizeof(parent_dir));
        parent_dir[sizeof(parent_dir) - 1] = '\0'; // Ensure null termination
        int line = 12;
        int sub_dir_line = 12;
        int max_y, max_x;
        getmaxyx(info_win, max_y, max_x);

        // Print parent directories on the left 
        DIR *dir = opendir(parent_dir);
        struct dirent *entry;
        if (dir != NULL) {
            wattron(info_win, A_BOLD | COLOR_PAIR(9));
            mvwprintw(info_win, 10, 2, " Parent Directories: ");
            wattroff(info_win, A_BOLD | COLOR_PAIR(9));

            int col = 2; // Starting column
            wattron(info_win, A_BOLD | COLOR_PAIR(2));
            while ((entry = readdir(dir)) != NULL) {
                if (entry->d_type == DT_DIR) {
                    // Exclude the current directory and the special entries "." and ".."
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        // Print the directory name
                        mvwprintw(info_win, line++, 2, "%s", entry->d_name);
                    }
                }
            }
            closedir(dir);
            wattroff(info_win, A_BOLD | COLOR_PAIR(2));
        } else {
            show_message(info_win, "Error opening parent directory.");
        }
        // Print subdirectories on the right
        dir = opendir(full_path);
        if (dir != NULL) {
            wattron(info_win, A_BOLD | COLOR_PAIR(2));
            while ((entry = readdir(dir)) != NULL && sub_dir_line < max_y - 1) {
                if (entry->d_type == DT_DIR) {
                    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                        mvwprintw(info_win, sub_dir_line++, max_x / 2, "  %s", entry->d_name);
                    }
                }
            }
            closedir(dir);
            wattroff(info_win, A_BOLD | COLOR_PAIR(2));
        } else {
            show_message(info_win, "Error opening directory.");
        }
    } else if (S_ISLNK(file_stat.st_mode)) {
        wattron(info_win, COLOR_PAIR(6));
        wprintw(info_win, "Symbolic Link");

        // Read the symlink target
        len = readlink(full_path, symlink_target, sizeof(symlink_target) - 1);
        if (len != -1) {
            symlink_target[len] = '\0'; // Null-terminate the string
            wattron(info_win, COLOR_PAIR(4));
            wprintw(info_win, "  to  %s", symlink_target);
            wattroff(info_win, COLOR_PAIR(4));
        } else {
            show_message(info_win, "Error reading symlink target.");
        }
        wattroff(info_win, COLOR_PAIR(6));
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

    clearLine(info_win, 10, 2);
    wattron(info_win, COLOR_PAIR(3));
    mvwprintw(info_win, 10, 2, "Permissions: ");
    wattroff(info_win, COLOR_PAIR(3));
    print_permissions(info_win, &file_stat);

    box(info_win, 0, 0);
    wrefresh(info_win);
}

void refreshMainWin(WINDOW *win, WINDOW *info_win, FileItem items[], int item_count, int highlight, const char *current_path, int show_hidden, int scroll_position, int height, int info_height, int info_width, int info_starty, int info_startx) {
  check_term_size(win, info_win);
  werase(win);
  if (info_win == NULL) {
      // Create info_win if it does not exist
      info_win = newwin(info_height, info_width, info_starty, info_startx);
      if (info_win == NULL) {
          perror("Failed to create info_win");
          exit(EXIT_FAILURE);
      }
      box(info_win, 0, 0);
  } else {
      box(info_win, 0, 0);
    }
  draw_colored_border(win, 2);
  print_items(win, items, item_count, highlight, current_path, show_hidden, scroll_position, height);
  wrefresh(win);
  if (item_count > 0) {
    werase(info_win);
    box(info_win, 0, 0);
    if (is_readable_extension(items[highlight].name) && !items[highlight].is_dir) {
        char full_path_info[PATH_MAX];
        snprintf(full_path_info, sizeof(full_path_info), "%s/%s", current_path, items[highlight].name); 
        display_file(info_win, full_path_info);
    } else {
        get_file_info(info_win, current_path, items[highlight].name);
    }
  }
}

int main(int argc, char* argv[]) {
  init_curses(); 

  int highlight = 0;
  FileItem items[MAX_ITEMS];
  int item_count = 0;
  char current_path[PATH_MAX];
  DirHistory history[MAX_HISTORY];
  int history_count = 0;
  int show_hidden = 0; // Flag to toggle showing hidden files
  int scroll_position = 0; // Position of the first visible item
  char *cur_user = get_current_user();
  if (argc == 2 && is_directory(argv[1])) {
    realpath(argv[1], current_path);
  } else {
    get_current_working_directory(current_path, sizeof(current_path));
  }
  // Create a new window with a border
  int startx = 0, starty = 0;
  int width = COLS / 2, height = LINES - 1;
  WINDOW * win = newwin(height, width, starty, startx);
  draw_colored_border(win, 2);
  list_dir(win, current_path, items, & item_count, show_hidden);

  int info_startx = COLS / 2, info_starty = 0;
  int info_width = COLS / 2, info_height = LINES - 1;
  WINDOW * info_win = newwin(info_height, info_width, info_starty, info_startx);
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
  log_message(LOG_LEVEL_DEBUG, "Entering as USER: %s",cur_user);

  while (true) { 
    int choice = getch();
    if (firstKeyPress) {
        refreshMainWin(win, info_win, items, item_count, highlight, current_path, show_hidden, scroll_position, height, info_height, info_width, info_starty, info_startx);
        show_term_message("", -1);
    }
    firstKeyPress = false;
    if (choice != ERR) {
      switch (choice) {
      case KEY_UP:
      case 'k':
        show_term_message("", -1);
        if (highlight > 0) {
          highlight--;
          if (highlight < scroll_position) {
            scroll_position--;
          }
        }
        break;
      case KEY_DOWN:
      case 'j':
        show_term_message("", -1);
        if (highlight < item_count - 1) {
          highlight++;
          if (highlight >= scroll_position + height - 8) {
            scroll_position++;
          }
        }
        break;
      
      case KEY_LEFT:
      case 'h':
          show_term_message("", -1);

          if (history_count > 0) {
              // Navigate using history
              history_count--;
              log_message(LOG_LEVEL_DEBUG, " [PARENT] Navigating back to %s", current_path);
              strcpy(current_path, history[history_count].path);
              highlight = history[history_count].highlight;
          } else {
              // Navigate to parent directory
              char parent_dir[1024];
              // Ensure parent_dir is initialized
              snprintf(parent_dir, sizeof(parent_dir), "%s", current_path);
              
              // Use dirname safely
              char *parent_path = dirname(parent_dir);
              log_message(LOG_LEVEL_DEBUG, " Checking out parent %s", parent_path);
              
              // Handle edge cases, such as navigating from the root directory
              if (strcmp(parent_path, "/") == 0 && strlen(current_path) > 1) {
                  // Special handling for root directory
                  strcpy(current_path, "/");
              } else {
                  snprintf(current_path, sizeof(current_path), "%s", parent_path);
                  log_message(LOG_LEVEL_DEBUG, " [PARENT] Navigating back to %s", current_path);
              }
              
              highlight = 0;
          }

          // List the contents of the new directory
          list_dir(win, current_path, items, &item_count, show_hidden);
          scroll_position = 0;
          break;

      
      case KEY_RIGHT:
      case 'l':
        show_term_message("", -1);
        char fullPath[MAX_PATH_LENGTH];
        snprintf(fullPath, MAX_PATH_LENGTH, "%s/%s", current_path, items[highlight].name);

        // Check access to the directory or file
        
        if (access(fullPath, R_OK) != 0) { 
            // Log the message safely
            log_message(LOG_LEVEL_ERROR, "[%s] Access denied for inode path %s: %s\n", cur_user, fullPath, strerror(errno));
            
            // Show the message to the user
            show_term_message("Access denied for this inode. Check log more details..", 1);
            break;
        }

        
        // Check access to the realPath
        if (items[highlight].is_dir) { 
              if (history_count < MAX_HISTORY) {
                  strcpy(history[history_count].path, current_path);
                  history[history_count].highlight = highlight;
                  history_count++;
              }
              strcat(current_path, "/");
              log_message(LOG_LEVEL_DEBUG, " [CHILD] Checking into %s", items[highlight].name);
              strcat(current_path, items[highlight].name);
              log_message(LOG_LEVEL_DEBUG, " [CHILD] Navigating into to %s", current_path);
              list_dir(win, current_path, items, &item_count, show_hidden);
              highlight = 0;
              scroll_position = 0;
            } else {
              if ((is_readable_extension(items[highlight].name) || !is_image(items[highlight].name)) && !items[highlight].is_dir) {
                firstKeyPress = true;
                launch_env_var(win, current_path, items[highlight].name, "EDITOR");
                /* Since we have set firstKeyPress to true, it will not wgetch(), rather it will just refresh everything back to how it was */
            } else if (is_image(items[highlight].name) && !items[highlight].is_dir) {
              firstKeyPress = true;
              launch_env_var(win, current_path, items[highlight].name, "VISUAL");
            } else {
              show_term_message("Cannot enter this directory/file.", 1);
            }
            check_term_size(win, info_win);
            werase(win);
        }
      break; 
      case 'G':
        highlight = item_count - 1; // will go to the last element in the currently displaying list
        break;
      case 'g':
        show_term_message(" [!] g", -1);
        halfdelay(100);
        char nextch = getch();
        if (nextch == 'g') {
          show_term_message("", -1);
          highlight = 0; // will go to the top most element in the current displaying list
        } else if (nextch == 't') { // GO TO func
          char destination_path[PATH_MAX];
          get_user_input_from_bottom(stdscr, destination_path, PATH_MAX, "goto", current_path);
          if (is_directory(destination_path)) {
            strcpy(current_path, destination_path);
            list_dir(win, current_path, items, & item_count, show_hidden);
            highlight = 0;
            scroll_position = 0;
          } else {
            log_message(LOG_LEVEL_ERROR, "The input `%s` is an invalid directory", destination_path);
            show_term_message("Invalid destination!", 1);
          }
          break;
        } else if (nextch == 'h') {
          show_term_message("", -1);
          char* home_dir = getenv("HOME");
          strcpy(current_path, home_dir);
          list_dir(win, current_path, items, & item_count, show_hidden);
          highlight = 0;
          scroll_position = 0;
          break;
        }
        else {
          show_term_message("Invalid command", 1);
        }
        break;
      case '.':
        show_hidden = !show_hidden; // Toggle show_hidden flag
        list_dir(win, current_path, items, & item_count, show_hidden);
        highlight = 0;
        scroll_position = 0;
        break;
      case 'H':
        strcpy(current_path, "/");
        list_dir(win, current_path, items, & item_count, show_hidden);
        highlight = 0;
        scroll_position = 0;
        break;
      case 'a': // Add file or directory
      {
        char name_input[NAME_MAX];
        get_user_input_from_bottom(stdscr, name_input, NAME_MAX, "add", current_path);

        if (strlen(name_input) > 0) {
          char timestamp[26];
          if (name_input[strlen(name_input) - 1] == '/') {
            // Create directory
            name_input[strlen(name_input) - 1] = '\0'; // Remove trailing slash
            int result = create_directory(current_path, name_input, timestamp);
            if (result == 0) {
              log_message(LOG_LEVEL_INFO, "Directory created successfully for `%s`", name_input);
              char msg[256];
              snprintf(msg, sizeof(msg), "  Directory '%s' created at %s.", name_input, timestamp);
              show_term_message(msg, 0);
            } else if (result == 1) {
              log_message(LOG_LEVEL_WARN, "Directory `%s` already exists", name_input);
              show_term_message("Directory already exists.", 1);
            } else {
              log_message(LOG_LEVEL_ERROR, "Error creating directory for %s", name_input);
              show_term_message("Error creating directory.", 1);
            }
          } else {
            // Create file
            int result = create_file(current_path, name_input, timestamp);
            if (result == 0) {
              char msg[256];
              log_message(LOG_LEVEL_INFO, "File created successfully for `%s`", name_input);
              snprintf(msg, sizeof(msg), "📄 File '%s' created at %s.", name_input, timestamp);
              show_term_message(msg, 0);
            } else if (result == 1) {
              log_message(LOG_LEVEL_WARN, "File `%s` already exists", name_input);
              show_term_message("File already exists.", 1);
            } else {
              log_message(LOG_LEVEL_ERROR, "Error creating file for %s", name_input);
              show_term_message("Error creating file.", 1);
            }
          }
          list_dir(win, current_path, items, & item_count, show_hidden);
          scroll_position = 0;
        } else {
          log_message(LOG_LEVEL_ERROR, "Null input given. Not creating anything...");
          show_term_message("Null input. Not adding anything..", 1);
        }

        break;
      }

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
            char * deldir = items[highlight].name;
            if (items[highlight].is_dir) {
              int result = remove_directory(current_path, items[highlight].name);
              if (result != 0) {
                log_message(LOG_LEVEL_ERROR, "Error deleting directory for `%s`", items[highlight].name);
                show_term_message("Error removing directory. Dir might be recursive.", 1);
              } else {
                char delmsg[256];
                log_message(LOG_LEVEL_INFO, "Directory `%s` deleted", deldir);
                snprintf(delmsg, sizeof(delmsg), "Directory '%s' deleted", deldir);
                show_term_message(delmsg, 0);
              }
            } else {
              char * delfile = items[highlight].name;
              int result = remove_file(current_path, items[highlight].name);
              if (result != 0) {
                log_message(LOG_LEVEL_ERROR, "Error removing file for `%s`", items[highlight].name);
                show_term_message("Error removing file.", 1);
              } else {
                char msg[256];
                log_message(LOG_LEVEL_INFO, "File `%s` deleted", delfile);
                snprintf(msg, sizeof(msg), "File '%s' deleted", delfile);
                show_term_message(msg, 0);
              }
            }
            list_dir(win, current_path, items, & item_count, show_hidden);
          }
        }
      }
      break;

      case 'D': {
        if (item_count > 0) {
          char confirm_msg[256];
          if (items[highlight].is_dir) {
            snprintf(confirm_msg, sizeof(confirm_msg), "[DANGER] Remove Directory recursively '%s'? (y/n)", items[highlight].name);
          } else {
            log_message(LOG_LEVEL_WARN, "Attempted to remove file `%s` recursively.", items[highlight].name);
            show_term_message("This command is for deleting recursive directories ONLY!", 1);
            break;
          }

          if (confirm_action(win, confirm_msg)) {
            char * deldir = items[highlight].name;
            if (items[highlight].is_dir) {
              int parent_fd = open(current_path, O_RDONLY | O_DIRECTORY);
              int result = remove_directory_recursive(current_path, items[highlight].name, parent_fd);
              if (result != 0) {
                log_message(LOG_LEVEL_ERROR, "Error removing directory `%s`", items[highlight].name);
                show_term_message("Error removing directory.", 1);
              } else {
                char delmsg[256];
                log_message(LOG_LEVEL_INFO, "Directory `%s` deleted recursively", deldir);
                snprintf(delmsg, sizeof(delmsg), "Directory '%s' deleted recursively", deldir);
                show_term_message(delmsg, 0);
              }
            }
            list_dir(win, current_path, items, & item_count, show_hidden);
          }
        }
      }
      break;
      case '/': // Find file or directory
      { 
        wattron(win, A_BOLD | COLOR_PAIR(7));
        mvwprintw(win, LINES - 3, (COLS / 2) - 75, "🔍 Search ON ");
        wattroff(win, A_BOLD | COLOR_PAIR(7));
        wrefresh(win);
        char query[NAME_MAX];
        get_user_input_from_bottom(stdscr, query, NAME_MAX, "search", current_path);

        if (strlen(query) > 0) {
          int start_index = highlight + 1;
          int found_index = find_item(query, items, item_count, & start_index, 1);
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
          int found_index = find_item(last_query, items, item_count, & start_index, 1);
          if (found_index != -1 && found_index != highlight) {
            highlight = found_index;
            if (highlight >= scroll_position + height - 8) {
              scroll_position = highlight - height + 8;
            } else if (highlight < scroll_position) {
              scroll_position = highlight;
            }
          } else {
            log_message(LOG_LEVEL_WARN, "No more NEXT occurances for `%s` found", last_query);
            show_term_message("No more occurrences found.", 1);
          }
        }
        break;
      case 'N':
        if (strlen(last_query) > 0) {
          int start_index = highlight - 1;
          int found_index = find_item(last_query, items, item_count, & start_index, -1);
          if (found_index != -1 && found_index != highlight) {
            highlight = found_index;
            if (highlight >= scroll_position + height - 8) {
              scroll_position = highlight - height + 8;
            } else if (highlight < scroll_position) {
              scroll_position = highlight;
            }
          } else {
            log_message(LOG_LEVEL_WARN, "No more PREV occurances for `%s` found", last_query);
            show_term_message("No previous occurrences found.", 1);
          }
        }
        break;

      case 'E':
        if (!items[highlight].is_dir) {
          // Check if it's an archive file
          const char * filename = items[highlight].name;
          if (strstr(filename, ".zip") || strstr(filename, ".tar") || strstr(filename, ".7z") || strstr(filename, ".jar")) {
            char full_path[PATH_MAX];
            snprintf(full_path, PATH_MAX, "%s/%s", current_path, filename);

            // Confirm extraction
            if (confirm_action(win, "Extract this archive? (y/n)")) {
              // Extract archive
              if (extract_archive(full_path) == 0) {
                log_message(LOG_LEVEL_INFO, "Extraction of `%s` successful", full_path);
                show_term_message("Extraction successful.", 0);
                // Update file list after extraction
                list_dir(win, current_path, items, & item_count, show_hidden);
                scroll_position = 0;
              } else {
                log_message(LOG_LEVEL_ERROR, "Extraction of `%s` failed", last_query);
                show_term_message("Extraction failed.", 1);
              }
            }
          } else {
            log_message(LOG_LEVEL_WARN, "File %s is not a supported archive format", filename);
            show_term_message("Not a supported archive format.", 1);
          }
        } else {
          log_message(LOG_LEVEL_ERROR, "Cannot extract %s as it is is directory", items[highlight].name);
          show_term_message("Cannot extract a directory.", 1);
        }
        break;

      case 'Z':
        if (items[highlight].is_dir) {
          // Check if the selected item is a directory
          const char * dirname = items[highlight].name;
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
              list_dir(win, current_path, items, & item_count, show_hidden);
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

              // Call the handle_rename function
              handle_rename(stdscr, full_path);

              // Update file list after renaming
              list_dir(win, current_path, items, &item_count, show_hidden);
              scroll_position = 0;
          } else {
              log_message(LOG_LEVEL_WARN, "No item selected for renaming");
              show_term_message("No item selected for renaming.", 1);
          }
          break;
      }
      case 'M':
      {
        halfdelay(100);
        char basefile[MAX_PATH_LENGTH];
        strcpy(basefile, items[highlight].name);
        char basepath[MAX_PATH_LENGTH];
        strcpy(basepath, current_path);
        char termMSG[256];
        snprintf(termMSG, 256, " [VISUAL]    Moving  %s     %s", basefile, basepath);

        show_term_message(termMSG, 0);
        do { 
          int nextch = getch();
            if (nextch == 'h' || nextch == KEY_LEFT) {
              if (history_count > 0) {
                history_count--;
                strcpy(current_path, history[history_count].path);
                highlight = history[history_count].highlight;
                list_dir(win, current_path, items, & item_count, show_hidden);
                scroll_position = 0;
              } else { // will allow for traversal to parents of get_current_working_directory (getcwd)
                  char parent_dir[1024];
                  strcpy(parent_dir, current_path);
                  strcpy(current_path, dirname(parent_dir));
                  list_dir(win, current_path, items, & item_count, show_hidden);
                  highlight = 0;
                  scroll_position = 0;
              }
            }
            else if (nextch == 'l' || nextch == KEY_RIGHT) {
              if (items[highlight].is_dir) {
                  if (history_count < MAX_HISTORY) {
                      strcpy(history[history_count].path, current_path);
                      history[history_count].highlight = highlight;
                      history_count++;
                  }
                  strcat(current_path, "/");
                  strcat(current_path, items[highlight].name);
                  list_dir(win, current_path, items, &item_count, show_hidden);
                  highlight = 0;
                  scroll_position = 0;
                } 
            }
            else if (nextch == 'k' || nextch == KEY_UP) {
              if (highlight > 0) {
                highlight--;
                if (highlight < scroll_position) {
                  scroll_position--;
                }
              }
            }
            else if (nextch == 'j' || nextch == KEY_DOWN) {
              if (highlight < item_count - 1) {
                highlight++;
                if (highlight >= scroll_position + height - 8) {
                  scroll_position++;
                }
              }
            }
            else if (nextch == '.') {
              show_hidden = !show_hidden; // Toggle show_hidden flag
              list_dir(win, current_path, items, & item_count, show_hidden);
              highlight = 0;
              scroll_position = 0;
            }
            else if (nextch == '/') {
              wattron(win, A_BOLD | COLOR_PAIR(7));
              mvwprintw(win, LINES - 3, (COLS / 2) - 75, "🔍 Search ON ");
              wattroff(win, A_BOLD | COLOR_PAIR(7));
              wrefresh(win);
              char query[NAME_MAX];
              get_user_input_from_bottom(stdscr, query, NAME_MAX, "search", current_path);

              if (strlen(query) > 0) {
                int start_index = highlight + 1;
                int found_index = find_item(query, items, item_count, & start_index, 1);
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
            else if (nextch == 10) {
              break;
            }
          refreshMainWin(win, info_win, items, item_count, highlight, current_path, show_hidden, scroll_position, height, info_height, info_width, info_starty, info_startx);

        } while (nextch != 10);
        move_file_or_dir(win, basepath, current_path,basefile);
        list_dir(win, current_path, items, & item_count, show_hidden);
        break;
      }
      case 10: {
        show_term_message("", -1);
        if (items[highlight].is_dir) {
          if (history_count < MAX_HISTORY) {
            strcpy(history[history_count].path, current_path);
            history[history_count].highlight = highlight;
            history_count++;
          }
          strcat(current_path, "/");
          strcat(current_path, items[highlight].name);
          list_dir(win, current_path, items, & item_count, show_hidden);
          highlight = 0;
          scroll_position = 0;
          break;
        } else {
          if (is_readable_extension(items[highlight].name)) {
            get_file_info_popup(win, current_path, items[highlight].name);
          } else {
            log_message(LOG_LEVEL_DEBUG, "Something went wrong...");
            show_term_message("Something went wrong....", 1);
          }
          break;
        }

      }
      case 'y': {
        char full_path[PATH_MAX];
        snprintf(full_path, PATH_MAX, "%s/%s", current_path, items[highlight].name);
        yank_selected_item(full_path);
        break;
      }
      case 'Y':
      {
        halfdelay(100);
        int createFile = 0;
        char basefile[MAX_PATH_LENGTH];
        strcpy(basefile, items[highlight].name);
        char basepath[MAX_PATH_LENGTH];
        snprintf(basepath, MAX_PATH_LENGTH, "%s/%s",current_path,basefile);
        char termMSG[256];
        snprintf(termMSG, 256, " [VISUAL]    Copying  %s to .... ", basepath);
        show_term_message(termMSG, 0);
        do { 
          int nextch = getch();
            if (nextch == 'h' || nextch == KEY_LEFT) {
              if (history_count > 0) {
                history_count--;
                strcpy(current_path, history[history_count].path);
                highlight = history[history_count].highlight;
                list_dir(win, current_path, items, & item_count, show_hidden);
                scroll_position = 0;
              } else { // will allow for traversal to parents of get_current_working_directory (getcwd)
                  char parent_dir[1024];
                  strcpy(parent_dir, current_path);
                  strcpy(current_path, dirname(parent_dir));
                  list_dir(win, current_path, items, & item_count, show_hidden);
                  highlight = 0;
                  scroll_position = 0;
              }
            }
            else if (nextch == 'l' || nextch == KEY_RIGHT) {
              if (items[highlight].is_dir) {
                  if (history_count < MAX_HISTORY) {
                      strcpy(history[history_count].path, current_path);
                      history[history_count].highlight = highlight;
                      history_count++;
                  }
                  strcat(current_path, "/");
                  strcat(current_path, items[highlight].name);
                  list_dir(win, current_path, items, &item_count, show_hidden);
                  highlight = 0;
                  scroll_position = 0;
                } 
            }
            else if (nextch == 'k' || nextch == KEY_UP) {
              if (highlight > 0) {
                highlight--;
                if (highlight < scroll_position) {
                  scroll_position--;
                }
              }
            }
            else if (nextch == 'j' || nextch == KEY_DOWN) {
              if (highlight < item_count - 1) {
                highlight++;
                if (highlight >= scroll_position + height - 8) {
                  scroll_position++;
                }
              }
            }
            else if (nextch == '.') {
              show_hidden = !show_hidden; // Toggle show_hidden flag
              list_dir(win, current_path, items, & item_count, show_hidden);
              highlight = 0;
              scroll_position = 0;
            }
            else if (nextch == '/') {
              wattron(win, A_BOLD | COLOR_PAIR(7));
              mvwprintw(win, LINES - 3, (COLS / 2) - 75, "🔍 Search ON ");
              wattroff(win, A_BOLD | COLOR_PAIR(7));
              wrefresh(win);
              char query[NAME_MAX];
              get_user_input_from_bottom(stdscr, query, NAME_MAX, "search", current_path);

              if (strlen(query) > 0) {
                int start_index = highlight + 1;
                int found_index = find_item(query, items, item_count, & start_index, 1);
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
            else if ((nextch == 10 || nextch == 'p')) {
              if (nextch == 'p' && !items[highlight].is_dir) {
                createFile = 1;
              }
              break;
            }
            /*else if (nextch == 'p' && !items[highlight].is_dir) {*/
            /*  break;*/
            /*}*/
          refreshMainWin(win, info_win, items, item_count, highlight, current_path, show_hidden, scroll_position, height, info_height, info_width, info_starty, info_startx);

        } while (nextch != 10);
          
        char destination_path[MAX_PATH_LENGTH];
        if (createFile != 1) {
          snprintf(destination_path, MAX_PATH_LENGTH, "%s/%s",current_path,basefile);
        } else {
          snprintf(destination_path, MAX_PATH_LENGTH, "%s/%s", current_path, items[highlight].name);
        }
        copyFileContents(basepath, destination_path);
        werase(win);
        wrefresh(win);
        werase(info_win);
        wrefresh(info_win);
        list_dir(win, current_path, items, & item_count, show_hidden);
        break;
        
      }
      case '?':
        displayHelp(win);
        break;
      case 'q':
        log_message(LOG_LEVEL_DEBUG, "================ LITEFM INSTANCE OVER =================");
        endwin();
        return 0;
      }
      // Update display after each key press
       refreshMainWin(win, info_win, items, item_count, highlight, current_path, show_hidden, scroll_position, height, info_height, info_width, info_starty, info_startx);
    }
  }

  endwin();
  return 0;
}
