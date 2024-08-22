// // // // // //
//             //
//   LITE FM   //
//             //
// // // // // //

/*
 * ---------------------------------------------------------------------------
 *  File:        filepreview.h
 *  Description: This header file provides function declarations for the file
 *                preview functionality within the LiteFM file manager. It
 *                includes methods for displaying file content in an NCurses
 *                window, checking file extensions for readability, and
 * formatting file sizes for display.
 *
 *  Author:      Siddharth Karanam
 *  Created:     <31/07/24>
 *
 *  Copyright:   2024 nots1dd. All rights reserved.
 *
 *  License:     <GNU GPL v3>
 *
 *  Notes:       The functions in this header are designed to work with NCurses
 *                to provide a text-based preview of files, suitable for file
 *                management applications. Ensure that file access permissions
 *                are correctly handled to avoid errors when attempting to read
 *                file content.
 *
 *  Revision History:
 *      <31/07/24> - Initial creation and function declarations added.
 *
 * ---------------------------------------------------------------------------
 */

#ifndef FILEPREVIEW_H
#define FILEPREVIEW_H

#include <ctype.h>
#include <libgen.h>
#include <limits.h> // For PATH_MAX
#include <limits.h>
#include <ncurses.h>
#include <stdio.h> // For FILE, fopen, fgets, fclose, perror
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "highlight.h"

// Constants
#define MAX_LINES 60        // Define the maximum number of lines to display
#define MAX_LINE_LENGTH 256 // Define the maximum line length

// Function Prototypes
const char* get_file_extension(const char* filename);
void        display_file(WINDOW* info_win, const char* filename);
const char* determine_file_type(const char* filename);
int         is_readable_extension(const char* filename);
const char* format_file_size(off_t size);
int         is_image(const char* filename);
int         is_audio(const char* filename);
void launch_env_var(WINDOW* win, const char* current_path, const char* filename, const char* type);
void print_permissions(WINDOW* info_win, struct stat* file_stat);
void display_archive_contents(WINDOW* info_win, const char* full_path, const char* file_ext);

#endif // FILEPREVIEW_H
