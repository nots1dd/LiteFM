// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // // 

/* BY nots1dd */

#include "../include/dircontrol.h"
#include "../include/cursesutils.h"
#include "../include/logging.h"

char* get_current_user() {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);

    if (pw == NULL) {
        perror("getpwuid");
        exit(EXIT_FAILURE);
    }

    return pw->pw_name;
}

char *get_hostname() {
    char buffer[100];
    if (gethostname(buffer, sizeof(buffer)) != 0) {
        perror("gethostname");
        return NULL;
    }

    // Allocate memory for the hostname string
    char *hostname = (char *)malloc(strlen(buffer) + 1);
    if (hostname == NULL) {
        perror("malloc");
        return NULL;
    }

    // Copy the hostname to the allocated memory
    strcpy(hostname, buffer);
    return hostname;
}
void get_current_working_directory(char *cwd, size_t size) {
    if (getcwd(cwd, size) == NULL) {
        strcpy(cwd, "/");
    }
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
int remove_directory_recursive(const char *base_path, const char *dirname, int parent_fd) {
    char full_path[PATH_MAX];
    snprintf(full_path, PATH_MAX, "%s/%s", base_path, dirname);
    int dir_fd = openat(parent_fd, full_path, O_RDONLY | O_DIRECTORY);
    if (dir_fd == -1) {
        show_term_message("Error opening directory.", 1);
        return -1;
    }

    DIR *dir = fdopendir(dir_fd);
    if (dir == NULL) {
        show_term_message("Error opening directory stream.", 1);
        close(dir_fd);
        return -1;
    }

    struct dirent *entry;
    int num_files_deleted = 0;
    int num_dirs_deleted = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        struct stat statbuf;
        if (fstatat(dir_fd, entry->d_name, &statbuf, 0) == 0) {
            if (S_ISDIR(statbuf.st_mode)) {
                // Recursively remove subdirectory
                if (remove_directory_recursive(full_path, entry->d_name, dir_fd) != 0) {
                    closedir(dir);
                    close(dir_fd);
                    return -1;
                }
                num_dirs_deleted++;
                char message[PATH_MAX];
                snprintf(message, PATH_MAX, "%s directory removed successfully.", entry->d_name);
                show_term_message(message, 0);
            } else {
                // Remove file
                if (unlinkat(dir_fd, entry->d_name, 0) != 0) {
                    char message[PATH_MAX];
                    snprintf(message, PATH_MAX, "Error removing file: %s", entry->d_name);
                    show_term_message(message, 1);
                    closedir(dir);
                    close(dir_fd);
                    return -1;
                }
                num_files_deleted++;
                char message[PATH_MAX];
                snprintf(message, PATH_MAX, "%s file removed successfully.", entry->d_name);
                show_term_message(message, 0);
            }
        } else {
            char message[PATH_MAX];
            snprintf(message, PATH_MAX, "Error getting status of file: %s", entry->d_name);
            show_term_message(message, 1);
            closedir(dir);
            close(dir_fd);
            return -1;
        }
    }

    closedir(dir);
    close(dir_fd);

    // Remove the now-empty directory
    if (unlinkat(parent_fd, dirname, AT_REMOVEDIR) != 0) {
        char message[PATH_MAX];
        snprintf(message, PATH_MAX, "Error removing directory: %s", dirname);
        show_term_message(message, 1);
        return -1;
    }
    num_dirs_deleted++;
    char message[PATH_MAX];
    snprintf(message, PATH_MAX, "%s dir removed successfully.", dirname);
    show_term_message(message, 0);

    char message2[PATH_MAX];
    snprintf(message2, PATH_MAX, "Deleted %d files and %d directories", num_files_deleted, num_dirs_deleted);
    show_term_message(message2, 0);

    return 0;
}

int rename_file_or_dir(const char *old_path, const char *new_name) {
    char new_path[PATH_MAX];
    
    // Construct the new path
    snprintf(new_path, sizeof(new_path), "%s/%s", dirname(strdup(old_path)), new_name);

    // Rename the file or directory
    if (rename(old_path, new_path) != 0) {
        perror("rename");
        return -1; // Return error code if rename fails
    }

    return 0; // Return success code
}

void move_file_or_dir(WINDOW *win, const char *current_path, const char *selected_item) {
    char destination[MAX_PATH_LENGTH];
    char full_current_path[MAX_PATH_LENGTH];
    char full_destination_path[MAX_PATH_LENGTH];

    // Construct the full path of the selected item
    snprintf(full_current_path, sizeof(full_current_path), "%s/%s", current_path, selected_item);

    // Get the destination path from the user
    get_user_input_from_bottom(stdscr, destination, MAX_PATH_LENGTH, "move", current_path);

    // Construct the full destination path
    snprintf(full_destination_path, sizeof(full_destination_path), "%s/%s", destination, selected_item);

    // Attempt to rename (move) the file or directory
    if (rename(full_current_path, full_destination_path) == 0) {
        char msg[256];
        log_message(LOG_LEVEL_INFO, "Moved `%s` to `%s`", selected_item, destination);
        snprintf(msg, sizeof(msg), "Successfully moved %s to %s", selected_item, destination);
        show_term_message(msg, 0);
    } else {
        log_message(LOG_LEVEL_ERROR, "Error moving file/dir `%s`", selected_item);
        show_term_message("Error moving file or directory.", 1);
    }
}

int is_directory(const char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return 0; // Path doesn't exist or some other error
    }
    return S_ISDIR(statbuf.st_mode);
}
