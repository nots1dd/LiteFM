// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        logging.h
 *  Description: This header file provides the interface for logging
 *               functionality in the LiteFM file manager. It includes declarations for
 *               functions to manage logging, including creating necessary directories and
 *               files, and writing log messages with various levels of
 * severity.
 *
 *  Author:      Siddharth Karanam
 *  Created:     <31/07/24>
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *                 **IMPORTANT NOTE**
 *
 *  If you do not have $HOME/.cache/litefm/log/litefm.log and did not use
 * `build.sh`, it may break LiteFM! Ensure that you have that directory created
 * by running `build.sh` for LiteFM to function correctly.
 *
 *  Notes:       The logging functions in this header manage log file creation,
 *               directory setup, and log message formatting. Ensure that your
 *               system permissions allow the creation and modification of
 *               files in the specified log directory.
 *
 *  Revision History:
 *      <31/07/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef LITEFM_LOGGING_H
#define LITEFM_LOGGING_H

#include <errno.h>
#include <limits.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

// Define the log directory and file paths relative to the home directory
#define LOG_DIR_RELATIVE_PATH ".cache/litefm/log"
#define LOG_FILE_RELATIVE_PATH ".cache/litefm/log/litefm.log"

// Logging levels
typedef enum
{
  LOG_LEVEL_INFO,
  LOG_LEVEL_WARN,
  LOG_LEVEL_ERROR,
  LOG_LEVEL_DEBUG
} LogLevel;

// Function declarations
const char* current_time_str();
const char* get_home_directory();
void        get_log_directory_path(char* buffer, size_t buffer_size);
void        get_log_file_path(char* buffer, size_t buffer_size);
void        ensure_log_directory_exists();
void        ensure_log_file_exists();
void        log_message(LogLevel level, const char* format, ...);

#endif // LITEFM_LOGGING_H
