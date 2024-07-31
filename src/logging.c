// // // // // // 
//             //
//   LITE FM   //
//             //
// // // // // //

/* By nots1dd */

/* 
   IMPORTANT NOTE ::
   
   IF YOU DO NOT HAVE $HOME/.cache/litefm/log/litefm.log,

   IT WILL BREAK LITEFM!!

   ENSURE THAT YOU HAVE THAT DIRECTORY CREATED (JUST RUN `build.sh`) FOR LITEFM TO WORK!
*/

#include "../logging.h"

// Function to get the current time as a string
const char *current_time_str() {
    static char time_str[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);
    return time_str;
}

// Function to get the home directory path
const char *get_home_directory() {
    const char *home = getenv("HOME");
    if (home == NULL) {
        home = getpwuid(getuid())->pw_dir;
    }
    return home;
}

// Function to get the full log directory path
void get_log_directory_path(char *buffer, size_t buffer_size) {
    const char *home = get_home_directory();
    snprintf(buffer, buffer_size, "%s/%s", home, LOG_DIR_RELATIVE_PATH);
}

// Function to get the full log file path
void get_log_file_path(char *buffer, size_t buffer_size) {
    const char *home = get_home_directory();
    snprintf(buffer, buffer_size, "%s/%s", home, LOG_FILE_RELATIVE_PATH);
}

// Function to create the log directory if it doesn't exist
void ensure_log_directory_exists() {
    char log_dir_path[PATH_MAX];
    get_log_directory_path(log_dir_path, sizeof(log_dir_path));

    struct stat st = {0};
    if (stat(log_dir_path, &st) == -1) {
        if (mkdir(log_dir_path, 0700) != 0) {
            perror("Failed to create log directory");
        }
    }
}

// Function to ensure the log file exists, creating it if necessary
void ensure_log_file_exists() {
    char log_file_path[PATH_MAX];
    get_log_file_path(log_file_path, sizeof(log_file_path));

    FILE *log_file = fopen(log_file_path, "a");
    if (log_file == NULL) {
        perror("Failed to open or create log file");
        return;
    }
    fclose(log_file);
}

// Function to log messages
void log_message(LogLevel level, const char *format, ...) {
    ensure_log_directory_exists();
    ensure_log_file_exists();

    char log_file_path[PATH_MAX];
    get_log_file_path(log_file_path, sizeof(log_file_path));

    FILE *log_file = fopen(log_file_path, "a");
    if (log_file == NULL) {
        perror("Failed to open log file");
        return;
    }

    const char *level_str;
    switch (level) {
        case LOG_LEVEL_INFO:
            level_str = "INFO";
            break;
        case LOG_LEVEL_WARN:
            level_str = "WARN";
            break;
        case LOG_LEVEL_ERROR:
            level_str = "ERROR";
            break;
        case LOG_LEVEL_DEBUG:
            level_str = "DEBUG";
            break;
        default:
            level_str = "UNKNOWN";
    }

    va_list args;
    va_start(args, format);
    fprintf(log_file, "[%s] [%s] ", current_time_str(), level_str);
    vfprintf(log_file, format, args);
    fprintf(log_file, "\n");
    va_end(args);

    fclose(log_file);
}
