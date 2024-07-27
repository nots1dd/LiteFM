#include <stdio.h>    // For snprintf
#include <stdlib.h>   // For exit
#include <string.h>   // For strcmp, strerror
#include <sys/stat.h> // For stat, S_ISDIR, S_ISREG
#include <errno.h>    // For errno
#include <unistd.h>   // For unlink, rmdir
#include <dirent.h>   // For opendir, readdir, closedir
#include <time.h>     // For time, localtime, strftime

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
        show_term_message("Error opening directory.",1);
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
                snprintf(message, PATH_MAX, "%s directory removed successfully.", file_path);
                show_term_message(message,0);
            } else {
                // Remove file
                if (unlink(file_path) != 0) {
                    char message[PATH_MAX];
                    snprintf(message, PATH_MAX, "Error removing file: %s", file_path);
                    show_term_message(message,1);
                    closedir(dir);
                    return -1;
                }
                num_files_deleted++;
                char message[PATH_MAX];
                snprintf(message, PATH_MAX, "%s file removed successfully.", file_path);
                show_term_message(message,0);
            }
        } else {
            char message[PATH_MAX];
            snprintf(message, PATH_MAX, "Error getting status of file: %s", file_path);
            show_term_message(message,1);
            closedir(dir);
            return -1;
        }
    }

    closedir(dir);

    // Remove the now-empty directory
    if (rmdir(full_path) != 0) {
        char message[PATH_MAX];
        snprintf(message, PATH_MAX, "Error removing directory: %s", full_path);
        show_term_message(message,1);
        return -1;
    }
    num_dirs_deleted++;
    char message[PATH_MAX];
    snprintf(message, PATH_MAX, "%s dir removed successfully.", full_path);
    show_term_message(message,0);

    char message2[PATH_MAX];
    snprintf(message2, PATH_MAX, "Deleted %d files and %d directories", num_files_deleted, num_dirs_deleted);
    show_term_message(message2,0);

    return 0;
}
